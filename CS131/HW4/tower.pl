% Part 1

constraints([], _).
constraints([H|T], N) :-
	length(H, N),
	fd_domain(H, 1, N),
	constraints(T, N).

flip_lr([], []).
flip_lr([AH|AT], [BH|BT]) :-
	reverse(AH, BH),
	flip_lr(AT, BT).

/*SWI Prolog implementation of transpose*/
/*http://www.swi-prolog.org/pack/file_details/xlibrary/prolog/transpose.pl?show=src*/
/*  Part of Extended Libraries for SWI-Prolog

    Author:        Edison Mera Menendez
    E-mail:        efmera@gmail.com
    WWW:           https://github.com/edisonm/xlibrary
    Copyright (C): 2014, Process Design Center, Breda, The Netherlands.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in
       the documentation and/or other materials provided with the
       distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
transpose([], L) :-
	once(maplist(=([]), L)).
transpose([C|Cs], L) :-
	deal_column(C, L, R),
	transpose(Cs, R).
deal_column([], L, L) :-
	once(maplist(=([]), L)).
deal_column([E|Es], [[E|R1]|L], [R1|R]) :-
	deal_column(Es, L, R).

flip_ud([], []).
flip_ud(A, B) :-
	reverse(A, C),
	transpose(C, B).

visible([], 0, _).
visible([H|T], Vis, Curr) :-
	H < Curr,
	visible(T, Vis, Curr).
visible([H|T], Vis, Curr) :-
	H > Curr,
	visible(T, New_vis, H),
	Vis is (New_vis + 1).

match_count([], []).
match_count([AH|AT], [BH|BT]) :-
	visible(AH, V, 0),
	BH = V,
	match_count(AT, BT).

tower(N, T, C) :-
	T = T_left,
	length(T_left, N),
	constraints(T_left, N),
	maplist(fd_all_different, T_left),
	transpose(T_left, T_up),
	maplist(fd_all_different, T_up),
	flip_lr(T_left, T_right),
	flip_ud(T_left, T_down),
	C = counts(UP, DOWN, LEFT, RIGHT),
	constraints([UP, DOWN, LEFT, RIGHT], N),
	maplist(fd_labeling, T),
	match_count(T_up, UP),
	match_count(T_down, DOWN),
	match_count(T_left, LEFT),
	match_count(T_right, RIGHT).


%---------------------------------------------------------------------------------------------------
% Part 2

% findall idea somewhat from https://stackoverflow.com/questions/7432156/prolog-list-of-numbers
base_list(N, L) :-
	findall(C, between(1, N, C), L).

repetition_list(0, _, []).
repetition_list(N, B, [B|T]) :-
	M is (N - 1),
	repetition_list(M, B, T),
	!.

unique_row([], [], [], []).
unique_row(B, [AH|AT], [RH|RT], [NH|NT]) :-
	member(RH, B),
	member(RH, AH),
	delete(B, RH, C),
	unique_row(C, AT, RT, NT),
	delete(AH, RH, NH).
	
all_unique_permutations([], _, _).
all_unique_permutations([H|T], B, C) :-
	unique_row(B, C, H, X),
	all_unique_permutations(T, B, X).

plain_tower(N, T, C) :-
	base_list(N, Base),
	T = T_left,
	length(T_left, N),
	repetition_list(N, Base, R),
	all_unique_permutations(T_left, Base, R),
	C = counts(UP, DOWN, LEFT, RIGHT),
	match_count(T_left, LEFT),
	flip_lr(T_left, T_right),
	match_count(T_right, RIGHT),
	transpose(T_left, T_up),
	match_count(T_up, UP),
	flip_ud(T_left, T_down),
	match_count(T_down, DOWN).

speedup(N) :-
	statistics(cpu_time, [T1, _]),
	tower(5, _,
         counts([4,1,2,3,3],
                [1,4,2,2,2],
                [2,2,3,2,1],
                [3,2,1,2,3])),
	tower(5, _,
         counts([1,3,5,2,2],
                [3,2,1,3,2],
                [1,2,3,2,2],
                [3,1,2,3,2])),
	tower(5, _,
         counts([2,3,5,1,3],
                [2,3,1,4,2],
                [2,1,2,4,2],
                [2,3,3,1,2])),
	statistics(cpu_time, [T2, _]),
	plain_tower(5, _,
         counts([4,1,2,3,3],
                [1,4,2,2,2],
                [2,2,3,2,1],
                [3,2,1,2,3])),
	plain_tower(5, _,
         counts([1,3,5,2,2],
                [3,2,1,3,2],
                [1,2,3,2,2],
                [3,1,2,3,2])),
	plain_tower(5, _,
         counts([2,3,5,1,3],
                [2,3,1,4,2],
                [2,1,2,4,2],
                [2,3,3,1,2])),
	statistics(cpu_time, [T3, _]),
	N is ((T3-T2) / (T2-T1)),
	!.

%---------------------------------------------------------------------------------------------------
% Part 3

ambiguous(N, C, T1, T2) :-
	tower(N, T1, C),
	tower(N, T2, C),
	T1 \= T2.

