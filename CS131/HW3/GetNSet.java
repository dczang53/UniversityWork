import java.util.concurrent.atomic.AtomicIntegerArray;

class GetNSet implements State {
    private AtomicIntegerArray value;
    private byte maxval;

    GetNSet(byte[] v) {
	int i = v.length;
	value = new AtomicIntegerArray​(i);
	i--;
	while(i >= 0) {
		value.set(i, (int) v[i]);
		i--;
		}
	maxval = 127;
	}

    GetNSet(byte[] v, byte m) {
	int i = v.length;
	value = new AtomicIntegerArray​(i);
	i--;
	while(i >= 0) {
		value.set(i, (int) v[i]);
		i--;
		}
	maxval = m;
	}

    public int size() {
	return value.length();
	}

    public byte[] current() {
	int i = value.length();
	byte[] ret = new byte[i];
	i--;
	while(i >= 0) {
		ret[i] = (byte) value.get(i);
		i--;
		}
	return ret;
	}

    public boolean swap(int i, int j) {
	int a = value.get(i);
	int b = value.get(j);
	if (a <= 0 || b >= maxval) {
	    return false;
	}
	value.set(i, --a);
	value.set(j, ++b);
	return true;
    }
}
