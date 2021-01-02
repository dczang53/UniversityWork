fun <T> everyNth(L: List<T>, N: Int): List<T> {
	return L.filterIndexed{index, _ -> (index + 1) % N == 0 }
}
