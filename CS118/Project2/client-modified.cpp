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
    static inline const int MAXIMUM_SEQUENCE_NUMBER{30720};
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


    int         my_socket_file_descriptor{};
    sockaddr_in my_address{};
    sockaddr_in their_address{};

    int next_sequence_number{};
    int next_expected_packet{};
    int send_base{};

    chrono::steady_clock::time_point timeout_time{};
    chrono::milliseconds             timeout_interval{30};
    bool                             is_timer_running{false};

    queue<Packet> packets_in_transit{};
    int           last_byte_sent{};
    int           last_byte_acked{};
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

        auto[sequence_number, acknowledgement_number, receive_window, length_of_data, ack_flag, syn_flag, fin_flag] = decode_header(
                your_header);

        vector<byte> file_data{};

        for (int i{HEADER_SIZE}; i < (HEADER_SIZE + length_of_data); ++i)
        {
            file_data.push_back(your_packet.at(i));
        }

        return tuple{sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data};
    }

    void send_acknowledgement(int length_of_data)
    {
        Packet to_send{next_sequence_number, increment_then_get_next_expected_packet(length_of_data), 100, 0, true, false, false, vector<byte>{}};
        send_packet(to_send); // TODO, what should sequence number be here?
    }

    void restart_clock()
    {
        timeout_time     = chrono::steady_clock::now() + timeout_interval;
        is_timer_running = true;
    }

    bool is_timeout()
    {
        return is_timer_running && chrono::steady_clock::now() < timeout_time;
    }

    void stop_clock()
    {
        is_timer_running = false;
    }

    bool is_their_window_free()
    {
        return (last_byte_sent - last_byte_acked <= their_receive_window);
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
            return decode_packet(buffer);
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

    void send_packet(Packet your_packet)
    {
        auto header{encode_header(your_packet.sequence_number, your_packet.acknowledgement_number,
                                  your_packet.receive_window, your_packet.length_of_data, your_packet.ack_flag,
                                  your_packet.syn_flag, your_packet.fin_flag)};

        auto packet{create_packet(header, your_packet.data)};

        array<byte, BUFFER_SIZE> buffer{};

        copy_n(packet.begin(), packet.size(), buffer.begin());

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
        array<pollfd, 1> poll_file_descriptors{};
        poll_file_descriptors.at(0).fd     = my_socket_file_descriptor;
        poll_file_descriptors.at(0).events = POLLIN;

        auto         from{your_data.begin()};
        vector<byte> sub_list{};

        while (true)
        {
            int poll_result{poll(poll_file_descriptors.data(), poll_file_descriptors.size(), 0)};

            if (poll_result == -1)
            {
                throw runtime_error("Something went wrong will poll.");
            }

            if (is_timeout()) // Timer expired
            {
                cout << "Timeout." << endl;
                send_packet(packets_in_transit.front());
                restart_clock();
            }

            if (same_bits(poll_file_descriptors.at(0).revents, POLLIN))
            {
                cout << "Acknowledgement received." << endl;
                auto result{wait_for_packet()};

                if (result)
                {
                    auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = result.value();
                    their_receive_window = receive_window;

                    while (!packets_in_transit.empty() &&
                           acknowledgement_number > packets_in_transit.front().sequence_number)
                    {
                        last_byte_acked = packets_in_transit.front().sequence_number;
                        packets_in_transit.pop();

                        if (!packets_in_transit.empty())
                        {
                            restart_clock();
                        }
                        else
                        {
                            stop_clock(); // We aren't sending any packets, so turn the timer off.
                        }
                    }


                    if (your_data.empty())
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
                Packet to_send{next_sequence_number, 0, 7, DATA_SIZE, false, false, false, sub_list};
                packets_in_transit.push(to_send);

                send_packet(to_send);
                from += DATA_SIZE;
                next_sequence_number += DATA_SIZE;
                last_byte_sent = next_sequence_number;
            }
            else if (is_their_window_free() && !your_data.empty())
            {
                if (!is_timer_running)
                {
                    restart_clock();
                }

                sub_list.assign(from, your_data.end());
                Packet to_send{next_sequence_number, 0, 7, your_data.end() - from, false, false, false, sub_list};
                packets_in_transit.push(to_send);
                send_packet(to_send); // TODO: Deal with window size

                next_sequence_number += your_data.end() - from;
                last_byte_sent = next_sequence_number;

                your_data.clear();
            }
        }


    }

    vector<byte> get_data()
    {
        vector<byte> data{};

        auto result{wait_for_packet()};

        if (result)
        {
            auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = result.value();
            if (sequence_number == next_expected_packet)
            {
                data.insert(data.end(), file_data.begin(), file_data.end());
                send_acknowledgement(file_data.size());
            }

        }
        else
        {
            throw runtime_error("We failed to read the acknowledgement packet.");
        }

        return data;
    }

    void establish_connection()
    {

        // TODO: What if there's a timeout?

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
		int current_sequence_number = next_sequence_number;
        Packet step_one_syn_packet{get_then_increment_sequence_number(1), 0, 0, 0, false, true, false, vector<byte>{}};
        send_packet(step_one_syn_packet);
		restart_clock();
		cout << "Sending packet " << current_sequence_number << " " << 0 << "SYN" << endl;
		struct pollfd rfd[1];
		rfd[0].fd = my_socket_file_descriptor;
		rfd[0].events = POLLIN;

		//poll for input for step-three while looping for step-two
		int status;
		while ((status = poll(rfd, 1, 0)) >= 0)
		{
			if (rfd[0].revents & POLLIN)
			{
				auto step_two_syn_packet{ wait_for_packet() };
				auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = step_two_syn_packet.value();
				cout << "Receiving packet " << acknowledgement_number << endl;
				if (syn_flag && ack_flag && (acknowledgement_number == next_sequence_number))
				{
					next_sequence_number=(sequence_number + 1) % MAXIMUM_SEQUENCE_NUMBER;
					break;
				}
			}
			if (is_timeout())
			{
				send_packet(step_one_syn_packet);
				restart_clock();
			}
		}

    }

	//not sure how to connect with next ack/data packet
	void initiate_request(string file_request)
	{
		send_text(file_request);
		restart_clock();
		cout << "Sending packet " << next_sequence_number << 0 << endl;
		while (1)
		{
			//process file name here, and break?
			if (is_timeout())
			{
				send_text(file_request);	//is this correct?
				restart_clock();
				cout << "Sending packet " << next_sequence_number << 0 << endl;
			}
		}
	}

    bool wait_for_syn_packet()
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
        else
        {
            auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = step_one_syn_packet.value();

            mt19937 random_number_generator;
            random_number_generator.seed(random_device()());
            uniform_int_distribution<mt19937::result_type> random_number_distribution{1, MAXIMUM_SEQUENCE_NUMBER};

            next_sequence_number = random_number_distribution(random_number_generator);
            next_expected_packet = sequence_number;

            Packet step_two_syn_packet{get_then_increment_sequence_number(1),
                                       increment_then_get_next_expected_packet(1),
                                       0, 0, true, true, false,
                                       vector<byte>{}};

            send_packet(step_two_syn_packet);
        }

        auto step_three_syn_packet{wait_for_packet()};
        if (!step_one_syn_packet)
        {
            throw runtime_error("We failed to read the third SYN packet.");
        }
        else
        {
            auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = step_three_syn_packet.value();
            increment_then_get_next_expected_packet(1);
            return true;
        }
    }

	void close_connection()
	{
		struct pollfd rfd[1];
		rfd[0].fd = my_socket_file_descriptor;
		rfd[0].events = POLLIN;

		//send FIN
		send_acknowledgement(0);
		restart_clock();
		cout << "Sending packet " << next_sequence_number << 0 << endl;

		//poll to detect ACK / resend FIN (upon retreiving ACK, close)
		int status;
		while ((status = poll(rfd, 1, 0)) >= 0)
		{
			if (rfd[0].revents & POLLIN)
			{
				auto ack_packet{ wait_for_packet() };
				auto[sequence_number, acknowledgement_number, receive_window, ack_flag, syn_flag, fin_flag, file_data] = ack_packet.value();
				if (ack_flag && (acknowledgement_number==next_expected_packet))
					return;
			}
			if (is_timeout())
			{
				send_acknowledgement(0);
				restart_clock();
				cout << "Sending packet " << next_sequence_number << 0 << endl;
			}
		}
	}
};

int main(int argc, char *argv[])
{

    if (argc < 4)
    {
        throw runtime_error("You need to give their IP address, their port number, and a filename.");
    }

    TCP_Connection my_connection{"0", argv[1], argv[2]};

    my_connection.establish_connection();
	my_connection.initiate_request(argv[3]);

	//assume that fin bit is being scanned for, and after last ack, it sends a fin packet

	my_connection.close_connection();    //use with conditional to initiate after last ACK possible sent (corresponding to FIN from server)

	/*
	NOTE:
	-modified:
	client:
	-main()
	-establish_connection()
	-close_connection()

	server:
	-main()
	-wait_for_syn_packet()
	-close_connection()
	-string file_name (new variable, dunno if needed)

	In both files:
	-cout cwnd not considered, set to 0
	-many be some errors with sequence and packet numbers (somewhat unsure if safe to modify or not, or just mixed up)
	-little-no compilation errors, so chances are logic errors
	
	*/



	/*
    my_connection.send_text("Moo cows.");
	*/
    return 0;
}
