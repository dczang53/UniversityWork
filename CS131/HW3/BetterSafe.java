import java.util.concurrent.locks.ReentrantLock;

class BetterSafe implements State {
    private byte[] value;
    private byte maxval;
    private ReentrantLock locks[];
    private ReentrantLock atomic = new ReentrantLock();

    BetterSafe(byte[] v) {
	value = v;
	maxval = 127;
	int i = v.length;
	locks = new ReentrantLock​[i];
	i--;
	while(i >= 0) {
		locks[i] = new ReentrantLock();
		i--;
		}
	}

    BetterSafe(byte[] v, byte m) {
	value = v;
	maxval = m;
	int i = v.length;
	locks = new ReentrantLock​[i];
	i--;
	while(i >= 0) {
		locks[i] = new ReentrantLock();
		i--;
		}
	}

    public int size() { return value.length; }

    public byte[] current() { return value; }

    public boolean swap(int i, int j) {
	//ReentrantLock a = locks[i];
	//ReentrantLock b = locks[j];
	atomic.lock();
	//a.lock();
	//b.lock();
	//atomic.unlock();
	if (value[i] <= 0 || value[j] >= maxval) {
	    //a.unlock();
	    //b.unlock();
	    atomic.unlock();
	    return false;
	}
	value[i]--;
	value[j]++;
	//a.unlock();
	//b.unlock();
	atomic.unlock();
	return true;
    }
}

//java UnsafeMemory BetterSafe 32 1000000 6 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3


