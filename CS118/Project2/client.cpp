#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#include <string>
#include <iostream>
#include <array>
#include <regex>
#include <tuple>
#include <iomanip>
#include <sstream>
#include <cstddef>
#include <optional>
#include <chrono>
#include <queue>
#include <random>
#include <map>
#include <fstream>

using namespace std;

bool same_bits(short your_bits, short your_other_bits)
{
    return (your_bits & your_other_bits) == your_other_bits;
}

class TCP_Connection
{
public:
    static inline const int BUFFER_SIZE{1024};
    static inline const int HEADER_SIZE{21};
    static inline const int DATA_SIZE{BUFFER_SIZE - HEADER_SIZE};
    static inline const int MAXIMUM_SEQUENCE_NUMBER{30720 - 1}; // We start counting at 0.
    static inline const int MAXIMUM_WINDOW_SIZE{5120};

    struct Packet
    {
        int          sequence_number;
        int          acknowledgement_number;
        int          receive_window;
        int          length_of_data;
        bool         ack_flag;
        bool         syn_flag;
        bool         fin_flag;
        vector<byte> data;
    };

private:

    int last_one_out_gets_the_lights{0};

    int              my_socket_file_descriptor{};
    sockaddr_in      my_address{};
    sockaddr_in      their_address{};
    array<pollfd, 1> poll_file_descriptors{};

    Packet           synack{};
    map<int, Packet> ack_history{}; // Maps from a packet's sequence number to the ACK that we sent for it.
    map<int, int>    packet_to_ack{}; // Maps from a packet's sequence number to the ACK number that acknowledges that packet.


    int next_sequence_number{};
    int next_expected_packet{};
    int send_base{};

    chrono::steady_clock::time_point timeout_time{};
    bool                             is_timer_running{false};

    deque<Packet> packets_in_transit{};
    int           their_receive_window{};


    auto decode_header(string your_header)
    {
        regex  packet_header_format{R"((.{5})(.{5})(.{4})(.{4})(.{1})(.{1})(.{1}))"};
        smatch matching_substrings{};
        if (regex_match(your_header, matching_substrings, packet_header_format))
        {
            int  sequence_number{stoi(matching_substrings[1])};
            int  acknowledgement_number{stoi(matching_substrings[2])};
            int  receive_window{stoi(matching_substrings[3])};
            int  length_of_data{stoi(matching_substrings[4])};
            bool ack_flag{static_cast<bool>(stoi(matching_substrings[5]))};
            bool syn_flag{static_cast<bool>(stoi(matching_substrings[6]))};
            bool fin_flag{static_cast<bool>(stoi(matching_substrings[7]))};

            return tuple{sequence_number, acknowledgement_number, receive_window, length_of_data, ack_flag, syn_flag,
                         fin_flag};
        }
        else
        {
            throw runtime_error("Something is wrong with the packet header's format.");
        }
    }

    auto
    encode_header(int sequence_number, int acknowledgement_number, int receive_window, int length_of_data,
                  bool ack_flag, bool syn_flag, bool fin_flag)
    {
        stringstream header_stream{};

        header_stream << setfill('0') <<
                      setw(5) << sequence_number <<
                      setw(5) << acknowledgement_number <<
                      setw(4) << receive_window <<
                      setw(4) << length_of_data <<
                      ack_flag << syn_flag << fin_flag;

        string header_string{header_stream.str()};

        return vector<char>(header_string.begin(), header_string.end());
    }

    auto create_packet(vector<char> header, vector<byte> data)
    {
        vector<byte> buffer{};

        for (auto character : header)
        {
            buffer.push_back(static_cast<byte>(character));
        }

        for (auto file_byte : data)
        {
            buffer.push_back(file_byte);
        }

        if (buffer.size() > BUFFER_SIZE)
        {
            throw runtime_error("Our created packet is too big.");
        }

        return buffer;
    }

    auto decode_packet(array<byte, BUFFER_SIZE> your_packet)
    {
        string your_header{};

        for (int i{}; i < HEADER_SIZE; ++i)
        {
            your_header += static_cast<char>(your_packet.at(i));
        }

        auto[sequence_number,
        acknowledgement_number,
        receive_window,
        length_of_data,
        ack_flag, syn_flag, fin_flag] = decode_header(your_header);

        vector<byte> file_data{};

        for (int i{HEADER_SIZE}; i < (HEADER_SIZE + length_of_data); ++i)
        {
            file_data.push_back(your_packet.at(i));
        }

        return tuple{sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data};
    }

    void send_acknowledgement(int length_of_data)
    {
        int previous_sequence_number{next_expected_packet};

        Packet to_send{next_sequence_number, // The next sequence number doesn't change--we're only sending an ACK.
                       increment_then_get_next_expected_packet(length_of_data),
                       MAXIMUM_WINDOW_SIZE, // Our program processes packets as soon as we see them, so we can always handle any packet that comes in.
                       0, // We're sending no data.
                       true, false, false,
                       vector<byte>{}};

        ack_history[previous_sequence_number] = to_send;

        send_packet(to_send, false);
    }

    void restart_clock(chrono::milliseconds timeout = chrono::milliseconds{500})
    {
        timeout_time     = chrono::steady_clock::now() + timeout;
        is_timer_running = true;
    }

    bool is_timeout()
    {
        return is_timer_running && chrono::steady_clock::now() >= timeout_time;
    }

    void stop_clock()
    {
        is_timer_running = false;
    }

    bool is_their_window_free()
    {
        return (packets_in_transit.size() < their_receive_window / BUFFER_SIZE);
    }

    int get_then_increment_sequence_number(int length_of_data_sent)
    {
        int current_sequence_number{next_sequence_number};
        next_sequence_number = (current_sequence_number + length_of_data_sent) % MAXIMUM_SEQUENCE_NUMBER;

        return current_sequence_number;
    }

    int increment_then_get_sequence_number(int length_of_data_sent)
    {
        int current_sequence_number{next_sequence_number};
        next_sequence_number = (current_sequence_number + length_of_data_sent) % MAXIMUM_SEQUENCE_NUMBER;

        return next_sequence_number;
    }

    int get_then_increment_next_expected_packet(int length_of_data_received)
    {
        int current_expected_packet{next_expected_packet};
        next_expected_packet = (current_expected_packet + length_of_data_received) % MAXIMUM_SEQUENCE_NUMBER;

        return current_expected_packet;
    }

    int increment_then_get_next_expected_packet(int length_of_data_received)
    {
        int current_expected_packet{next_expected_packet};
        next_expected_packet = (current_expected_packet + length_of_data_received) % MAXIMUM_SEQUENCE_NUMBER;

        return next_expected_packet;
    }

    int get_next_expected_packet(int packet_number, int length_of_data_received)
    {
        return (packet_number + length_of_data_received) % MAXIMUM_SEQUENCE_NUMBER;
    }

public:

    explicit TCP_Connection(string our_port_number)
    {
        my_socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (my_socket_file_descriptor < 0)
        {
            throw runtime_error("Creating the socket failed.");
        }

        my_address.sin_family      = AF_INET;
        my_address.sin_addr.s_addr = htonl(INADDR_ANY);
        my_address.sin_port        = htons(stoi(our_port_number));

        if (bind(my_socket_file_descriptor, reinterpret_cast<sockaddr *>(&my_address), sizeof(my_address)) < 0)
        {
            throw runtime_error("Binding the socket failed.");
        }

        poll_file_descriptors.at(0).fd     = my_socket_file_descriptor;
        poll_file_descriptors.at(0).events = POLLIN;
    }

    TCP_Connection(string our_port_number, string their_ip_address, string their_port_number)
            : TCP_Connection{our_port_number}
    {
        their_address.sin_family = AF_INET;
        their_address.sin_port   = htons(stoi(their_port_number));

        if (inet_pton(AF_INET, their_ip_address.c_str(), &(their_address.sin_addr)) < 0)
        {
            throw runtime_error("We could not convert their IP address.");
        }

    }

    optional<tuple<int, int, int, bool, bool, bool, vector<byte>>> wait_for_packet()
    {
        array<byte, BUFFER_SIZE> buffer{};

        socklen_t their_address_length{sizeof(their_address)};

        auto length_of_message{recvfrom(my_socket_file_descriptor, buffer.data(), buffer.size(), 0,
                                        reinterpret_cast<sockaddr *>(&their_address), &their_address_length)};


        if (length_of_message > 0)
        {
            auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = decode_packet(
                    buffer);

            if (syn_flag)
            {
                cout << "Receiving packet " << get_next_expected_packet(sequence_number, 1) << endl;
            }
            else
            {
                cout << "Receiving packet  " << get_next_expected_packet(sequence_number, file_data.size()) << endl;
            }


            return tuple{sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag,
                         file_data};
        }
        else if (length_of_message == 0)
        {
            return nullopt;
        }
        else
        {
            throw runtime_error("Something went wrong with recvfrom().");
        }
    }

    void send_packet(Packet your_packet, bool is_retransmission)
    {
        auto header{encode_header(your_packet.sequence_number,
                                  your_packet.acknowledgement_number,
                                  your_packet.receive_window,
                                  your_packet.length_of_data,
                                  your_packet.ack_flag, your_packet.syn_flag, your_packet.fin_flag)};

        auto packet{create_packet(header, your_packet.data)};

        array<byte, BUFFER_SIZE> buffer{};
        copy_n(packet.begin(), packet.size(), buffer.begin());

        cout << "Sending packet" << " " <<
             your_packet.sequence_number << " ";

        if (is_retransmission)
        {
            cout << "Retransmission" << " ";
        }
        if (your_packet.syn_flag)
        {
            cout << "SYN" << " ";
        }
        if (your_packet.fin_flag)
        {
            cout << "FIN";
        }

        cout << endl;

        if (sendto(my_socket_file_descriptor, buffer.data(), buffer.size(), 0,
                   reinterpret_cast<sockaddr *>(&their_address), sizeof(their_address)) == -1)
        {
            throw runtime_error("We couldn't send them the packet.");
        }
    }

    void send_text(string your_text)
    {
        vector<byte> data{};

        for (auto character : your_text)
        {
            data.push_back(static_cast<byte>(character));
        }

        send_data(data);
    }

    void send_data(vector<byte> your_data)
    {
        auto         from{your_data.begin()};
        vector<byte> sub_list{};
        stop_clock();

        if (your_data.empty())
        {
            if (!is_timer_running)
            {
                restart_clock();
            }

            Packet to_send{next_sequence_number,
                           0, // There's no ACK on this packet, so we don't give a number.
                           MAXIMUM_WINDOW_SIZE, // Explained in the send_acknowledgement() function.
                           0,
                           false, false, true, // We're done sending our garbage, so the FIN flag is on.
                           your_data};

            packets_in_transit.push_back(to_send);
            packet_to_ack[to_send.sequence_number] = get_next_expected_packet(to_send.sequence_number,
                                                                              to_send.length_of_data);

            send_packet(to_send, false);
        }

        while (true)
        {
            int poll_result{poll(poll_file_descriptors.data(), poll_file_descriptors.size(), 0)};

            if (poll_result == -1)
            {
                throw runtime_error("Something went wrong will poll.");
            }

            if (is_timeout()) // Timer expired
            {
                send_packet(packets_in_transit.front(), true);
                restart_clock();

                if (!packets_in_transit.empty() && packets_in_transit.back().fin_flag == true)
                {
                    last_one_out_gets_the_lights++;
                }

                if (last_one_out_gets_the_lights == 117)
                {
                    break;
                }
            }

            if (same_bits(poll_file_descriptors.at(0).revents, POLLIN))
            {
                auto result{wait_for_packet()};

                if (result)
                {
                    auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = result.value();
                    their_receive_window = receive_window;

                    if (!packets_in_transit.empty() &&
                        acknowledgement_number == packet_to_ack[packets_in_transit.front().sequence_number])
                    {
                        packets_in_transit.pop_front();

                        if (!packets_in_transit.empty())
                        {
                            restart_clock();
                        }
                        else
                        {
                            stop_clock(); // We aren't sending any packets, so turn the timer off.
                        }
                    }


                    if (packets_in_transit.empty() && your_data.empty())
                    {
                        break; // We don't have any more data to send and the receiver got all our packets, so exit from sending.
                    }
                }
                else
                {
                    throw runtime_error("We failed to read the acknowledgement packet.");
                }
            }


            if (same_bits(poll_file_descriptors.at(0).revents, POLLHUP) ||
                same_bits(poll_file_descriptors.at(0).revents, POLLERR))
            {
                break;
            }

            if (is_their_window_free() && your_data.end() - from > DATA_SIZE)
            {
                if (!is_timer_running)
                {
                    restart_clock();
                }


                sub_list.assign(from, from + DATA_SIZE);
                Packet to_send{next_sequence_number,
                               0,
                               MAXIMUM_WINDOW_SIZE,
                               DATA_SIZE,
                               false, false, false,
                               sub_list};

                packets_in_transit.push_back(to_send);
                packet_to_ack[to_send.sequence_number] = get_next_expected_packet(to_send.sequence_number,
                                                                                  to_send.length_of_data);

                send_packet(to_send, false);
                from += DATA_SIZE;
                increment_then_get_sequence_number(DATA_SIZE);
            }
            else if (is_their_window_free() && !your_data.empty())
            {
                if (!is_timer_running)
                {
                    restart_clock();
                }

                sub_list.assign(from, your_data.end());
                Packet to_send{next_sequence_number,
                               0, // There's no ACK on this packet, so we don't give a number.
                               MAXIMUM_WINDOW_SIZE, // Explained in the send_acknowledgement() function.
                               your_data.end() - from,
                               false, false, true, // We're done sending our garbage, so the FIN flag is on.
                               sub_list};

                packets_in_transit.push_back(to_send);
                packet_to_ack[to_send.sequence_number] = get_next_expected_packet(to_send.sequence_number,
                                                                                  to_send.length_of_data);

                send_packet(to_send, false);
                increment_then_get_sequence_number(your_data.end() - from);

                your_data.clear();
            }
        }


    }

    vector<byte> get_data()
    {
        vector<byte> data{};
        bool         done_getting_data{};
        stop_clock();

        while (true)
        {
            int poll_result{poll(poll_file_descriptors.data(), poll_file_descriptors.size(), 0)};
            if (poll_result == -1)
            {
                throw runtime_error("Something went wrong will poll.");
            }

            if (is_timeout())
            {
                break;
            }

            if (same_bits(poll_file_descriptors.at(0).revents, POLLIN))
            {
                auto result{wait_for_packet()};
                if (result)
                {
                    auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = result.value();
                    if (sequence_number == next_expected_packet)
                    {
                        if (fin_flag && !is_timer_running)
                        {
                            restart_clock(chrono::milliseconds{500} * 2);
                        }

                        data.insert(data.end(), file_data.begin(), file_data.end());
                        send_acknowledgement(file_data.size());
                    }
                    else if (syn_flag) // Uh oh, the other person didn't get our SYNACK from way back. That's OK, just send it here.
                    {
                        send_packet(synack, true);
                    }
                    else
                    {
                        if (ack_history.count(sequence_number))
                        {
                            send_packet(ack_history[sequence_number], true);
                        }
                    }

                }
                else
                {
                    throw runtime_error("We failed to read the acknowledgement packet.");
                }
            }

            if (same_bits(poll_file_descriptors.at(0).revents, POLLHUP) ||
                same_bits(poll_file_descriptors.at(0).revents, POLLERR))
            {
                throw runtime_error("Connection was closed abruptly.");
            }

        }

        return data;
    }

    string get_text()
    {
        auto      raw_data{get_data()};
        string    result{};
        for (auto one_byte : raw_data)
        {
            result += static_cast<char>(one_byte);
        }

        return result;
    }

    void establish_connection()
    {

//        int          sequence_number;
//        int          acknowledgement_number;
//        int          receive_window;
//        int          length_of_data;
//        bool         ack_flag;
//        bool         syn_flag;
//        bool         fin_flag;
//        vector<byte> data;


        mt19937 random_number_generator;
        random_number_generator.seed(random_device()());
        uniform_int_distribution<mt19937::result_type> random_number_distribution{1, MAXIMUM_SEQUENCE_NUMBER};
        next_sequence_number = random_number_distribution(random_number_generator);

        Packet step_one_syn_packet{get_then_increment_sequence_number(1),
                                   0,
                                   MAXIMUM_WINDOW_SIZE,
                                   0,
                                   false, true, false, vector<byte>{}};

        send_packet(step_one_syn_packet, false);
        restart_clock();

        while (true)
        {
            int poll_result{poll(poll_file_descriptors.data(), poll_file_descriptors.size(), 0)};

            if (poll_result == -1)
            {
                throw runtime_error("Something went wrong will poll.");
            }

            if (is_timeout()) // Timer expired
            {
                send_packet(step_one_syn_packet, true);
                restart_clock();
            }

            if (same_bits(poll_file_descriptors.at(0).revents, POLLIN))
            {
                auto step_two_syn_packet{wait_for_packet()};

                if (step_two_syn_packet)
                {
                    auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = step_two_syn_packet.value();
                    if (acknowledgement_number == next_sequence_number)
                    {
                        next_expected_packet = sequence_number;
                        their_receive_window = receive_window;

                        // Our next expected packet is one after the sequence number that they gave us.
                        increment_then_get_next_expected_packet(1);
                        stop_clock();

                        break;
                    }
                }
                else
                {
                    throw runtime_error("We failed to read the acknowledgement packet.");
                }
            }
            if (same_bits(poll_file_descriptors.at(0).revents, POLLHUP) ||
                same_bits(poll_file_descriptors.at(0).revents, POLLERR))
            {
                throw runtime_error("Connection was closed abruptly.");
            }
        }

    }

    void wait_for_syn_packet()
    {
        //        int          sequence_number;
        //        int          acknowledgement_number;
        //        int          receive_window;
        //        int          length_of_data;
        //        bool         ack_flag;
        //        bool         syn_flag;
        //        bool         fin_flag;
        //        vector<byte> data;

        auto step_one_syn_packet{wait_for_packet()};
        if (!step_one_syn_packet)
        {
            throw runtime_error("We failed to read the first SYN packet.");
        }

        auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = step_one_syn_packet.value();

        mt19937 random_number_generator;
        random_number_generator.seed(random_device()());
        uniform_int_distribution<mt19937::result_type> random_number_distribution{1, MAXIMUM_SEQUENCE_NUMBER};

        next_expected_packet = sequence_number;
        increment_then_get_next_expected_packet(1);

        their_receive_window = receive_window;

        next_sequence_number = random_number_distribution(random_number_generator);

        Packet step_two_syn_packet{get_then_increment_sequence_number(1),
                                   next_expected_packet,
                                   MAXIMUM_WINDOW_SIZE,
                                   0,
                                   true, true, false,
                                   vector<byte>{}};

        synack = step_two_syn_packet;
        send_packet(step_two_syn_packet, false);
    }
};


void write_file(vector<byte> your_bytes)
{
    ofstream out_file{"received.data", ios_base::trunc | ios_base::binary};

    if (!out_file.good())
    {
        throw runtime_error("We couldn't open the file for writing.");
    }

    for (auto one_byte : your_bytes)
    {
        out_file.put(static_cast<char>(one_byte));
    }
}

vector<byte> read_file(string file_name)
{
    ifstream file_stream{file_name, ios_base::binary};
    if (file_stream.fail())
    {
        return vector<byte>{};
    }

    vector<byte> file_contents{};
    char         next{};

    while (file_stream.get(next))
    {
        file_contents.push_back(static_cast<byte>(next));
    }

    return file_contents;
}


int main(int argc, char *argv[])
{

    if (argc < 4)
    {
        throw runtime_error("You need to give their IP address, their port number, and a filename.");
    }

    TCP_Connection my_connection{"0", argv[1], argv[2]};

    my_connection.establish_connection();

    my_connection.send_text(argv[3]);
    auto my_file{my_connection.get_data()};
    write_file(my_file);

    return 0;
}
