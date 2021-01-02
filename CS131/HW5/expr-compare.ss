;#lang racket

;---------------------------------------------------------------------------------------------------
; Part 1

;taken from post @311 on Piazza
; https://piazza.com/class/jqr2kb45gw2jt?cid=311
(define lambda_symbol (string->symbol "\u03BB"))

(define (sngl-compare x y sub1 sub2)
	(if (equal? x y)
		(let ([m (assoc x sub1)] [n (assoc y sub2)])
			(cond
				[(and m n) (list 'if '% (string->symbol (string-append (symbol->string x) "!" (symbol->string (cdr m)))) (string->symbol (string-append (symbol->string (cdr n)) "!" (symbol->string y))))]
				[m (list 'if '% (string->symbol (string-append (symbol->string x) "!" (symbol->string (cdr m)))) y)]
				[n (list 'if '% x (string->symbol (string-append (symbol->string (cdr n)) "!" (symbol->string y))))]
				[else x]
			)

#|
			(if m
				(list 'if '% (string->symbol (string-append (symbol->string x) "!" (symbol->string (cdr m)))) (string->symbol (string-append (symbol->string (cdr m)) "!" (symbol->string x))))
				x
			)
|#


		)
		(cond
			[(and (boolean? x) (boolean? y)) (if x '% (list 'not '%))]
			[else
				(let ([m (assoc x sub1)] [n (assoc y sub2)])
					(cond
						[(and m n) (if (and (equal? (cdr m) y) (equal? (cdr n) x)) (string->symbol (string-append (symbol->string x) "!" (symbol->string y))) (list 'if '% (string->symbol (string-append (symbol->string x) "!" (symbol->string (cdr m)))) (string->symbol (string-append (symbol->string (cdr n)) "!" (symbol->string y)))))]
						[m (list 'if '% (string->symbol (string-append (symbol->string x) "!" (symbol->string (cdr m)))) y)]
						[n (list 'if '% x (string->symbol (string-append (symbol->string (cdr n)) "!" (symbol->string y))))]
						[else (list 'if '% x y)]
					)
				)
			]
		)
	)



)

; special case given in Piazza post @320
; https://piazza.com/class/jqr2kb45gw2jt?cid=320
(define (symb-bind x y)
	(if (not (equal? x y)) (list (cons x y)) '())
)

(define (make-binds x y)
	(if (equal? x '())
		'()
		(let ([a (car x)] [b (car y)])
			(if (or (not (symbol? a)) (equal? a b))
				(make-binds (cdr x) (cdr y))
				(cons (cons a b) (make-binds (cdr x) (cdr y)))
			)
		)
	)
)

(define (list-compare x y sub1 sub2 ft)
	(if (equal? x '())
		'()
		(let ([a (car x)] [b (car y)])
			(cond
				[(and (list? a) (list? b) (= (length a) (length b))) (cons (list-compare a b sub1 sub2 #t) (list-compare (cdr x) (cdr y) sub1 sub2 #f))]
				[(and (not (list? a)) (not (list? b)))
					(if (and ft (symbol? a) (symbol? b))
						(cond
							[(and (or (equal? a 'lambda) (equal? a lambda_symbol)) (or (equal? b 'lambda) (equal? b lambda_symbol)))
								(if (> (length x) 1)
									(let ([c (cadr x)] [d (cadr y)])
										(cond
											[(and (list? c) (list? d) (= (length c) (length d))) (cons (if (and (equal? a 'lambda) (equal? b 'lambda)) 'lambda lambda_symbol) (list-compare (cdr x) (cdr y) (make-binds c d) (make-binds d c) #f))]
											[(and (symbol? c) (symbol? d)) (cons (if (and (equal? a 'lambda) (equal? b 'lambda)) 'lambda lambda_symbol) (list-compare (cdr x) (cdr y) (symb-bind c d) (symb-bind d c) #f))]
											[else (list 'if '% x y)]
										)
									)
									(list (if (and (equal? a 'lambda) (equal? b 'lambda)) 'lambda lambda_symbol))
								)
							]
							[(and (= (length x) 2) (or (equal? a 'quote) (equal? b 'quote)))
								(if (equal? a 'quote)
									(if (equal? b 'quote)
										(sngl-compare x y sub1 sub2)
										(list (list 'if '% a b) (sngl-compare (cadr x) (cadr y) sub1 sub2))
									)
									(list (list 'if '% a b) (sngl-compare (cadr x) (cadr y) sub1 sub2))
								)
							]
							[(and (or (= (length x) 3) (= (length x) 4)) (not (equal? a b)) (or (equal? a 'if) (equal? b 'if)))
								(list 'if '% x y)
							]
							[else (cons (sngl-compare a b sub1 sub2) (list-compare (cdr x) (cdr y) sub1 sub2 #f))]
						)
						(cons (sngl-compare a b sub1 sub2) (list-compare (cdr x) (cdr y) sub1 sub2 #f))
					)
				]
				[else (cons (list 'if '% a b) (list-compare (cdr x) (cdr y) sub1 sub2 #f))]
			)
		)
	)
)

(define (expr-compare x y)
	(cond
		[(and (list? x) (list? y) (= (length x) (length y))) (list-compare x y '() '() #t)]
		[(and (not (list? x)) (not (list? y))) (sngl-compare x y '() '())]
		[else (list 'if '% x y)]
	)
)

;---------------------------------------------------------------------------------------------------
; Part 2

(define (test-expr-compare x y)
	(let ([z (expr-compare x y)]) (and (equal? (eval x) (eval (list 'let '([% #t]) z))) (equal? (eval y) (eval (list 'let '([% #f]) z)))))
)

;---------------------------------------------------------------------------------------------------
; Part 3

(define test-expr-x
	'(list
		(if (= ((lambda (a b) (* a b)) 6 2) 12) #f #t)	; constant literals, variable references, procedure calls, true/false, lambda special form, if special form
		"Hello"						; constant literal (string)
		(quote (x y))					; quote special form
	)
)

(define test-expr-y
	'(list
		(if (= ((lambda (b a) (/ b a)) 6 2) 3) #t #f)	
		"World"
		(quote (y x))
	)
)

;---------------------------------------------------------------------------------------------------
; Test Cases Below

#|
(expr-compare 12 12)
(expr-compare 12 20)
(expr-compare #t #t)
(expr-compare #f #f)
(expr-compare #t #f)
(expr-compare #f #t)
(expr-compare 'a '(cons a b))
(expr-compare '(cons a b) '(cons a b))
(expr-compare '(cons a b) '(cons a c))
(expr-compare '(cons (cons a b) (cons b c)) '(cons (cons a c) (cons a c)))
(expr-compare '(cons a b) '(list a b))
(expr-compare '(list) '(list a))
(expr-compare ''(a b) ''(a c))
(expr-compare '(quote (a b)) '(quote (a c)))
(expr-compare '(quoth (a b)) '(quoth (a c)))
(expr-compare '(if x y z) '(if x z z))
(expr-compare '(if x y z) '(g x y z))
(expr-compare '((lambda (a) (f a)) 1) '((lambda (a) (g a)) 2))
(expr-compare '((lambda (a) (f a)) 1) '((λ (a) (g a)) 2))
(expr-compare '((lambda (a) a) c) '((lambda (b) b) d))
(expr-compare ''((λ (a) a) c) ''((lambda (b) b) d))
(expr-compare '(+ #f ((λ (a b) (f a b)) 1 2)) '(+ #t ((lambda (a c) (f a c)) 1 2)))
(expr-compare '((λ (a b) (f a b)) 1 2) '((λ (a b) (f b a)) 1 2))
(expr-compare '((λ (a b) (f a b)) 1 2) '((λ (a c) (f c a)) 1 2))
(expr-compare '((lambda (a) (eq? a ((λ (a b) ((λ (a b) (a b)) b a)) a (lambda (a) a)))) (lambda (b a) (b a))) '((λ (a) (eqv? a ((lambda (b a) ((lambda (a b) (a b)) b a)) a (λ (b) a)))) (lambda (a b) (a b))))
|#

#|
(expr-compare '(lambda (a b) a) '(lambda (a) a))
(expr-compare '(lambda (a b) a) '(lambda (a . b) a))
(expr-compare '(lambda a a) '(lambda (a) a))
(expr-compare '(lambda a a) '(lambda b b))
(expr-compare '(lambda (a b) a) '(lambda (a b) b))
(expr-compare '(lambda (a b) (a b c)) '(lambda (a b) (c b a)))
(expr-compare '(lambda (a b) (a b c)) '(lambda (a b) a))
(expr-compare '(lambda (a b) (b a)) '(lambda (b a) (a b)))
|#

;(test-expr-compare test-expr-x test-expr-y)



