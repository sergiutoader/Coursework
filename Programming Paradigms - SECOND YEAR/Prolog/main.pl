% Toader Sergiu-Cristian, 325CB
% Tema Prolog

:- use_module(tables).
:- use_module(check_predicates).

head([], []).
head([H|_], H).

tail([], []).
tail([_|T], T).

% implementarea predicatului eval
eval(table(Str), R) :- table_name(Str, R).
eval(tprint(Q), R) :- eval(Q, R), print_table_op(R).
eval(select(Columns, Q), R) :- eval(Q, R1), select_op(R1, Columns, R).
eval(join(Pred, Cols, Q1, Q2), R) :- eval(Q1, R1), eval(Q2, R2),
									 join_op(Pred, Cols, R1, R2, R).
eval(tfilter(S, G, Q), [H|R]) :- eval(Q, [H|T]), filter_op(T, S, G, R).

% se evalueaza Table, se filtreaza dupa sufixul "escu"
% se filtreaza dupa media la AA si PP, se filtreaza dupa media generala
eval(complex_query1(Table), [H|R]) :-
	eval(Table, [H|T]),
	filter_op(T, [_,Nume,_,_,_,_,_], (substring(Nume, "escu")), R1),
	filter_op(R1, [_,_,AA,PP,_,_,_], (average([AA,PP], Avg1), Avg1 > 6 ), R2),
	filter_op(R2, [_,_,AA,PP,PC,PA,POO], (average([AA,PP,PC,PA,POO], Avg2),
		Avg2 > 5), R).
% se construieste noul header, se evalueaza tabelul movies, se filtreaza
% intrarile din tabel dupa genul filmului.
% se evalueaza tabelul ratings si se filtreaza intrarile din movies in functie
% de nota filmului extrasa din tabelul ratings
% se adauga nota respectiva la fiecare linie
eval(complex_query2(Genre, Min, Max), [H|R]) :-
	H = ["movie_id", "title", "genres", "rating"],
	eval(table(movies), [_|T]),
	filter_op(T, [_,_,Genres], (substring(Genres, Genre)), R1),
	eval(table(ratings), [_|T1]),
	filter_op(R1, [ID, _, _], (getRating(T1, ID, Rez),
		Rez >= Min, Rez =< Max), R2),
	appendRatings(R2, T1, R).


%%% PRINT %%%%

% functie care calculeaza lungimile string-urilor dintr-o lista
getLengths([], []).
getLengths([H|T], [X|R]) :- string_length(H, X), getLengths(T, R).
% maparea functiei anterioare pe o lista de liste
getAllLengths(L, R) :- maplist(getLengths, L, R).

% primeste o lista de dimensiuni pentru fiecare element din tabela si
% calculeaza lungimea maxima pe fiecare coloana
getMaxLengths([[]|_], []).
getMaxLengths(L, [H|T]) :- maplist(head, L, R1), maplist(tail, L, R2),
			max_list(R1, H), getMaxLengths(R2, T). 

print_table_op(Table) :- getAllLengths(Table, R1),
					getMaxLengths(R1, R2),
					make_format_str(R2, Format),
					maplist(format(Format), Table).

%%%% SELECT %%%%

% input : coloanele ce trebuie selectate, prima linie dn tabel
% output: indicii coloanelor ce trebuie selectate
indexList([], _, []).
indexList([H|T], List, [H1|R]) :- nth0(H1, List, H), indexList(T, List, R).

% predicatul nth0 built-in in prolog, cu parametrii 1 si 2 in ordine inversa
% (pentru a putea fi folosit in maplist)
myNth(List, Idx, R) :- nth0(Idx, List, R).

% selecteaza elementele de pe linie de pe pozitiile date
selectOneLine(IdxList, List, R) :- maplist(myNth(List), IdxList, R).

% aplicarea predicatului selectOneLine pe toate liniile tabelului
selectAllLines(IdxList, List, R) :- maplist(selectOneLine(IdxList), List, R).

% operatia select
% extrage indicii coloanelor din prima linie,
% aplica predicatul selectAllLines pe tot tabelul
select_op([H|T], Args, R) :- indexList(Args, H, IdxList),
							 selectAllLines(IdxList, [H|T], R).

%%%% JOIN %%%%

% predicat pentru operatia join
% se combina liniile din tabelul 1, respectiv tabelul 2, aplicandu-se in
% acelasi timp operatia op. Se ataseaza la acest tabel noile denumiri pentru
% coloane
join_op(Op, Newcols, [_|T1], [_|T2], R) :- maplist(Op, T1, T2, Rp),
										   append([Newcols], Rp, R).


%%%% FILTER %%%%

% se parcurge fiecare linie in parte, se realizeaza unificarea intre Vars si H
% (linia curenta) astfel, variabilele testate de Pred vor fi unificate la
% elementele corespunzatoare de pe linie.
% In cazul in care conditia nu este respectata,
% nu se introduce linia in tabelul final
% in caz contrar, aceasta este adaugata la rezultat
filter_op([],_,_,[]).
filter_op([H|T], Vars, Pred, R) :- not((Vars = H, Pred)),
								   filter_op(T, Vars, Pred, R), !.
filter_op([H|T], Vars, Pred, [H|R]) :- filter_op(T, Vars, Pred, R).



%%%% COMPLEX QUERIES %%%%

% 1
% predicat care calculeaza media elementelor dintr-o lista
average(L, R):-  sumlist(L, S), length(L, Len), Len > 0, R is S / Len.
% predicat care verifica daca Substr este un subsir al lui Str
substring(Str,SubStr) :- sub_string(Str,_,_,_,SubStr).

%2
% itereaza prin liniile tabelului pana la cea cu ID-ul dat
% determina rating-ul acelui film
getRating([[_,_,IDp, Rez]|_], ID, Rez) :- ID = IDp, !.
getRating([_|T], ID, Rez) :- getRating(T, ID, Rez).

% adauga pe o coloana noua pentru fiecare linie nota sa
appendRatings(L, Ratings, R) :- maplist(addToLine(Ratings), L, R).
% adauga nota pentru un film
% mai intai se calculeaza nota. apoi se face append la sfarsitul liniei
addToLine(Ratings, [ID | T], R) :- getRating(Ratings, ID, Rez),
								   append([ID | T], [Rez], R). 



% functii auxiliare pentru make_format_str
plus5(X,Y):- Y is X + 5.
make_format_str(MaxRowLen,Str) :- maplist(plus5,MaxRowLen,Rp),
								  aux_format(Rp,Str).
aux_format([H],R) :- string_concat("~t~w~t~",H,R1),
					 string_concat(R1,"+~n",R),
					 !.
aux_format([H|T],R) :- string_concat("~t~w~t~",H,R1),
					   string_concat(R1,"+ ",R2),
					   aux_format(T,Rp),
					   string_concat(R2,Rp,R).