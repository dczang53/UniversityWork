fun main(args: Array<String>) {
	var list1 = listOf(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
	var result1 = everyNth(list1, 3)
	var list2 = listOf("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10")
	var result2 = everyNth(list2, 3)
	var list3: List<Any> = listOf()
	var result3 = everyNth(list3, 3)
	if(result1.equals(listOf(2, 5, 8)))
		println("Test 1 pass")
	else
		print("Test 1 fail")
	if(result2.equals(listOf("2", "5", "8")))
		println("Test 2 pass")
	else
		print("Test 2 fail")
	if(result3.equals(list3))
		println("Test 3 pass")
	else
		print("Test 3 fail")
}
