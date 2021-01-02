(*Symbol Type Declaration*)
type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal

(*Tree Type Declaration*)
type ('nonterminal, 'terminal) parse_tree =
  | Node of 'nonterminal * ('nonterminal, 'terminal) parse_tree list
  | Leaf of 'terminal

(*Helper Function for Extracting Terminal Nonterminal Values*)
let is_nterm token = match token with
  | N _ -> true
  | T _ -> false

let extract_T token = match token with
  | T var -> var
  | N _ -> failwith "Invalid operation on nonterminal symbol"

let extract_N token = match token with
  | N var -> var
  | T _ -> failwith "Invalid operation on terminal symbol"



(*------------------------------------------------------------------------------------------------*)



(*#1: convert_grammar gram1*)
let convert_grammar gram1 =
let rec get_token_lists prod_list nterm = match prod_list with
  | [] -> []
  | hd::tl -> if ((fst hd) = nterm) then ((fst hd)::(get_token_lists tl nterm)) else (get_token_lists tl nterm) in
(fst gram1, get_token_lists (snd gram1));;



(*#2: parse_tree_leaves tree*)
let rec parse_tree_leaves tree = match tree with
  | Node (_, desc_list) -> traverse_list desc_list
  | Leaf var -> [var]
and
traverse_list node_list = match node_list with
  | [] -> []
  | hd::tl -> (parse_tree_leaves hd)@(traverse_list tl);;



(*#3: make_matcher gram*)
let make_matcher gram =
let rec prefix_match_rule rules current_list prefix = match (current_list, prefix) with
  | ([], []) -> true
  | (_, []) -> false
  | ([], _) -> false
  | (hd::tl, h::t) -> if ((List.length prefix) < (List.length current_list)) then (false) else (if (is_nterm hd) then (try_sub_list (List.filter (fun x -> not (x = [hd])) (rules (extract_N hd))) rules current_list prefix) else (if ((extract_T hd) = h) then (prefix_match_rule rules tl t) else (false)))
and
try_sub_list sub_list rules current_list prefix = match (sub_list, current_list) with
  | ([], _) -> false
  | (hd::tl, []) -> if (prefix_match_rule rules hd prefix) then (true) else (try_sub_list tl rules current_list prefix)
  | (hd::tl, h::t) -> if (prefix_match_rule rules (hd @ t) prefix) then (true) else (try_sub_list tl rules current_list prefix)
in
let rec test_all_prefix grammar acceptor prefix suffix = match suffix with
  | [] -> if (try_sub_list (List.filter (fun x -> not (x = [N (fst grammar)])) ((snd grammar) (fst grammar))) (snd grammar) [] prefix) then (acceptor suffix) else (None)
  | hd::tl -> (
	match (test_all_prefix grammar acceptor (prefix @ [hd]) tl) with
	  | None -> if (try_sub_list (List.filter (fun x -> not (x = [N (fst grammar)])) ((snd grammar) (fst grammar))) (snd grammar) [] prefix) then (acceptor suffix) else (None)
	  | var -> var
	) in
let get_match grammar acceptor fragment = test_all_prefix grammar acceptor [] fragment in
(get_match gram);;



(*#4: make_parser gram*)
let make_parser gram =
let root = fst gram in
let rules = snd gram in
let rec try_lists rule_list node_prefix_stack node_suffix_stack suffix_len frag = match rule_list with
  | [] -> []
  | hd::tl -> if ((suffix_len + (List.length hd) - 1) > (List.length frag)) then (try_lists tl node_prefix_stack node_suffix_stack suffix_len frag) else (
	match (create_list ([]::node_prefix_stack) (hd::node_suffix_stack) (suffix_len + (List.length hd) - 1) frag) with
	  | [] -> try_lists tl node_prefix_stack node_suffix_stack suffix_len frag
	  | var -> var
	)
and
create_list node_prefix_stack node_suffix_stack suffix_len frag = match node_suffix_stack with
  | [] -> failwith "Invalid node_suffix_stack detected: Stack should never be empty"
  | hd::tl -> (
	match hd with
	  | [] -> (
		match tl with
		  | [] -> if (frag = []) then (node_prefix_stack) else ([])
		  | x::y -> (
			match x with
			  | [] -> failwith "Invalid suffix detected: Empty suffix"
			  | m::n -> if (is_nterm m) then (create_list (((List.hd (List.tl node_prefix_stack)) @ [Node ((extract_N m), (List.hd node_prefix_stack))])::(List.tl (List.tl node_prefix_stack))) (n::y) suffix_len frag) else (failwith "Invalid suffix detected: Missing N")
			)
		)
	  | a::b -> (
		match frag with
		  | [] -> []
		  | m::n -> (
			match a with
			  | T _ -> if (m = (extract_T a)) then (create_list (((List.hd node_prefix_stack) @ [Leaf m])::(List.tl node_prefix_stack)) (b::tl) (suffix_len - 1) n) else ([])
			  | N _ -> (
				match (try_lists (List.filter (fun x -> not (x = [a])) (rules (extract_N a))) node_prefix_stack node_suffix_stack suffix_len frag) with
				  | [] -> []
				  | var -> var
				)
			)
		)
	)
in
let optional_tree fragment = match (create_list [[]] [[N root]] 1 fragment) with
  | [] -> None
  | [[]] -> None
  | x -> Some (List.hd (List.hd x))
in
optional_tree;;



