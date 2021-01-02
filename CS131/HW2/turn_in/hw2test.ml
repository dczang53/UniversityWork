(*Grammar and functions for tests*)
let accept_all string = Some string
(*taken from hw 2 spec*)

type my_nonterminals =
  | Sentence | NounPhrase | VerbPhrase | Noun | Verb | Adj | Adv

let my_grammar =
  (Sentence,
   function
     | Sentence ->
         [[N Sentence];
          [N NounPhrase; N VerbPhrase; T "and"; N VerbPhrase];
          [N NounPhrase; N VerbPhrase; T "but"; N VerbPhrase];
          [N NounPhrase; N VerbPhrase]]
     | NounPhrase ->
         [[N NounPhrase];
          [N NounPhrase; T "and"; N NounPhrase];
          [T "the"; N Adj; N Noun];
          [T "the"; N Noun]]
     | VerbPhrase ->
         [[N VerbPhrase];
          [N Adv; N Verb];
          [N Verb; N Adv];
          [N Verb]]
     | Noun ->
	 [[T "dog"];
          [T "cat"];
          [T "fox"]]
     | Verb ->
	 [[T "ate"];
          [T "drank"];
          [T "slept"]]
     | Adj ->
	 [[N Adj; N Adj];
          [T "black"];
          [T "white"];
          [T "dirty"]]
     | Adv ->
	 [[T "loudly"];
          [T "quietly"];
          [T "cautiously"]])

let match_frag = ["the"; "black"; "dirty"; "dog"; "ate"; "quietly"; "and"; "the"];;
let parse_frag = ["the"; "black"; "dirty"; "dog"; "and"; "the"; "white"; "fox"; "ate"; "quietly"; "and"; "loudly"; "slept"];;

(*#5: test case of #3 make_matcher_test*)
let make_matcher_test = ((make_matcher my_grammar accept_all match_frag) = Some ["and"; "the"])

(*#6: test case of #4 make_parser_test*)
let make_parser_test =
  match make_parser my_grammar parse_frag with
    | Some tree -> parse_tree_leaves tree = parse_frag
    | _ -> false




