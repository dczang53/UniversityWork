class File
{
public:
	File(string f) { f_name = f; }
	virtual ~File() {}
	string name() const { return f_name; }
	virtual void open() const = 0;
	virtual void redisplay() const { cout << "refresh the screen"; }
private:
	string f_name;
};

class TextMsg : public File
{
public:
	TextMsg(string t) : File(t) {}
	~TextMsg() { cout << "Destroying " << File::name() << ", a text message" << endl; }
	virtual void open() const { cout << t_open; }
private:
	string t_open = "open text message";
};

class Video : public File
{
public:
	Video(string v, int n = 0) : File(v) { v_time = n; }
	~Video() { cout << "Destroying " << File::name() << ", a video" << endl; }
	virtual void open() const { cout << v_open1 << v_time << v_open2; }
	virtual void redisplay() const { cout << "replay video"; }
private:
	string v_open1 = "play ";
	int v_time;
	string v_open2 = " second video";
};

class Picture : public File
{
public:
	Picture(string p) : File(p) {}
	~Picture() { cout << "Destroying the picture " << File::name() << endl; }
	virtual void open() const { cout << p_open; }
private:
	string p_open = "show picture";
};