(*Symbol Type Declaration*)
type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal;;

(*1: subset a b*)
let rec subset a b =
if a = [] then true
else (if (List.exists (fun x -> x = (List.hd a)) b) then subset (List.tl a) b else false);;

(*2: equal_sets a b*)
let equal_sets a b = (subset a b) && (subset b a);;

(*3: set_union a b*)
let set_union a b =
let rec set x = if x = [] then x else (if (List.exists (fun y -> y = (List.hd x)) (List.tl x)) then set (List.tl x) else (List.hd x)::(set (List.tl x))) in
let c = set a in
let d = set b in
let rec combine m n = if m = [] then n else (if (List.exists (fun z -> z = (List.hd m)) n) then combine (List.tl m) n else (List.hd m)::(combine (List.tl m) n)) in
if c = [] then d
else combine c d;;

(*4: set_intersection a b*)
let rec set_intersection a b =
let rec set x = if x = [] then x else (if (List.exists (fun y -> y = (List.hd x)) (List.tl x)) then set (List.tl x) else (List.hd x)::(set (List.tl x))) in
let c = set a in
let d = set b in
let rec check m n = if m = [] then m else (if (List.exists (fun z -> z = (List.hd m)) n) then (List.hd m)::(check (List.tl m) n) else (check (List.tl m) n)) in
if c = [] then c
else check c d;;

(*5: set_diff a b*)
let set_diff a b =
let rec set x = if x = [] then x else (if (List.exists (fun y -> y = (List.hd x)) (List.tl x)) then set (List.tl x) else (List.hd x)::(set (List.tl x))) in
let c = set a in
let d = set b in
let rec diff m n = if m = [] then m else (if (List.exists (fun z -> z = (List.hd m)) n) then diff (List.tl m) n else (List.hd m)::(diff (List.tl m) n)) in
if c = [] then c
else diff c d;;

(*6: computed_fixed_point eq f x*)
let rec computed_fixed_point eq f x =
if(eq x (f x)) then x
else (computed_fixed_point eq f (f x));;

(*7: filter_reachable g*)
let filter_reachable g =
let root = fst g in
let full_prod_list = snd g in
let check_new_nonterm nonterm_list symb =
match symb with
  | T _ -> false
  | N var -> not (List.exists (fun x -> x = var) nonterm_list) in
let rec traverse_symb_list nonterm_list symb_list =
match symb_list with
  | [] -> []
  | hd::tl -> if (check_new_nonterm nonterm_list hd) then (let (N i) = hd in i::(traverse_symb_list (i::nonterm_list) tl)) else traverse_symb_list nonterm_list tl in
let rec find_new_nonterm nonterm_list prod_list =
match prod_list with
  | [] -> nonterm_list
  | hd::tl -> if (List.exists (fun x -> x = (fst hd)) nonterm_list) then (let n = traverse_symb_list nonterm_list (snd hd) in if n = [] then find_new_nonterm nonterm_list tl else find_new_nonterm (nonterm_list @ n) full_prod_list) else find_new_nonterm nonterm_list tl in
let rec filter nonterm_list prod_list =
match prod_list with
  | [] -> []
  | hd::tl -> if (List.exists (fun x -> x = (fst hd)) nonterm_list) then (hd)::(filter nonterm_list tl) else filter nonterm_list tl in
(root, filter (find_new_nonterm [fst g] full_prod_list) full_prod_list);;



