all: test.jar

check: test.jar
	@java -jar test.jar
	@printf "\n"

test.jar: everyNth.kt test.kt
	@kotlinc everyNth.kt test.kt -include-runtime -d test.jar

clean:
	@rm -f test.jar
