(*functions on http://web.cs.ucla.edu/classes/winter19/cs131/hw/hw1.html used as reference*)

(*1: subset a b*)
let my_subset_test0 = not (subset [1;2;3] [3;6;8;4;5;1])
let my_subset_test1 = subset ["hs";"";"efd";"abc"] ["abc";"hs";"";"eiow";"efd";"shit";"sefds"]
let my_subset_test2 = subset [1;3;7;3;7] [4;1;3;7;1;7]
let my_subset_test3 = subset [1;2;3] [1;2;3]
let my_subset_test4 = subset [] [1;2;3]
let my_subset_test5 = not (subset [1;2;3] [])
let my_subset_test6 = subset [] []

(*2: equal_sets a b*)
let my_equal_sets_test0 = equal_sets [5;3;5] [5;5;5;5;3;3]
let my_equal_sets_test1 = equal_sets ["hs";"";"efd";"abc"] ["abc";"hs";"";"";"efd"]
let my_equal_sets_test2 = not (equal_sets [5;3;5;4] [5;5;5;5;3;3])
let my_equal_sets_test3 = not (equal_sets [5;3;5] [5;5;5;5;4;3;3])
let my_equal_sets_test4 = not (equal_sets [] [1;2;3])
let my_equal_sets_test5 = not (equal_sets [1;2;3] [])
let my_equal_sets_test6 = equal_sets [] []

(*3: set_union a b*)
let my_set_union_test0 = equal_sets (set_union [1;2;3] [1;2;3]) [1;2;3]
let my_set_union_test1 = equal_sets (set_union [1] [2;3]) [1;2;3]
let my_set_union_test2 = equal_sets (set_union [1;2;1] [2;3;2]) [1;2;3]
let my_set_union_test3 = equal_sets (set_union ["a";"d";"b"] ["c";"b"]) ["a";"b";"c";"d"]
let my_set_union_test4 = equal_sets (set_union [] [1;2;3]) [1;2;3]
let my_set_union_test5 = equal_sets (set_union [1;2;3] []) [1;2;3]
let my_set_union_test6 = equal_sets (set_union [] []) []

(*4: set_intersection a b*)
let my_set_intersection_test0 = equal_sets (set_intersection [1;2;3] [1;2;3]) [1;2;3]
let my_set_intersection_test1 = equal_sets (set_intersection [1] [2;3]) []
let my_set_intersection_test2 = equal_sets (set_intersection [1;2;1] [2;3;2]) [2]
let my_set_intersection_test3 = equal_sets (set_intersection ["a";"d";"b"] ["c";"b"]) ["b"]
let my_set_intersection_test4 = equal_sets (set_intersection [1;2;3] []) []
let my_set_intersection_test5 = equal_sets (set_intersection [] [1;2;3]) []
let my_set_intersection_test6 = equal_sets (set_intersection [] []) []


(*5: set_diff a b*)
let my_set_diff_test0 = equal_sets (set_diff [1;2;3] [1;2;3]) []
let my_set_diff_test1 = equal_sets (set_diff [1] [2;3]) [1]
let my_set_diff_test2 = equal_sets (set_diff [1;2;1] [2;3;2]) [1]
let my_set_diff_test3 = equal_sets (set_diff ["a";"d";"b"] ["c";"b"]) ["a";"d"]
let my_set_diff_test4 = equal_sets (set_diff [1;2;3] []) [1;2;3]
let my_set_diff_test5 = equal_sets (set_diff [] [1;2;3]) []
let my_set_diff_test6 = equal_sets (set_diff [] []) []

(*6: computed_fixed_point eq f x*)
let my_computed_fixed_point_test0 = computed_fixed_point (=) (fun x -> x ** x) 2. = infinity

(*7: filter_reachable g*)
type food =
  | Instant_Noodles | Soup | Flavoring | Water | Noodles | Flour | MSG
let ingredients =
  [MSG, [T "msg"];
   Water, [T "water"];
   Instant_Noodles, [N Soup; N Noodles];
   Soup, [N Water];
   Flavoring, [N Water];
   Flour, [T "flour"];
   Noodles, [N Flour];
   Instant_Noodles, [N Water; N Noodles; N Flavoring]]

let my_filter_reachable_test0 = filter_reachable (Instant_Noodles, ingredients) = (Instant_Noodles, List.tl ingredients)
let my_filter_reachable_test1 = filter_reachable (Noodles, ingredients) = (Noodles, [Flour, [T "flour"]; Noodles, [N Flour]])



