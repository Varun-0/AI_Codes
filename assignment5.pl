male(varun).
male(manish).
male(devbrat).
male(phulchand).
male(vickram).
male(vivek).
male(ved).
male(prakash).
male(vinod).
male(pramod).
male(brahmand).
male(vaibhav).

female(asha).
female(shreya).
female(chandrakala).
female(urmila).
female(khushboo).
female(ranjita).
female(anamika).
female(riya).
female(srushti).


parent(manish, varun).
parent(manish, shreya).
parent(asha, varun).
parent(asha, shreya).

parent(devbrat, manish).
parent(chandrakala, manish).
parent(devbrat, vivek).
parent(chandrakala, vivek).
parent(devbrat, vickram).
parent(chandrakala, vickram).
parent(devbrat, ranjita).
parent(chandrakala, ranjita).

parent(phulchand, asha).
parent(urmila, asha).
parent(phulchand, prakash).
parent(urmila, prakash).
parent(phulchand, vinod).
parent(urmila, vinod).
parent(phulchand, pramod).
parent(urmila, pramod).

parent(vickram, brahmand).
parent(khushboo, brahmand).
parent(vickram, srushti).
parent(khushboo, srushti).

parent(vivek, riya).
parent(anamika, riya).



father(X, Y) :- male(X), parent(X, Y).
mother(X, Y) :- female(X), parent(X, Y).

sibling(X, Y) :- parent(Z, X), parent(Z, Y), X \= Y.

brother(X, Y) :- sibling(X, Y), male(X).
sister(X, Y) :- sibling(X, Y), female(X).

child(X, Y) :- parent(Y, X).

grandparent(X, Y) :- parent(X, Z), parent(Z, Y).
grandfather(X, Y) :- grandparent(X, Y), male(X).
grandmother(X, Y) :- grandparent(X, Y), female(X).

uncle(X, Y) :- brother(X, Z), parent(Z, Y).
aunt(X, Y) :- sister(X, Z), parent(Z, Y).

cousin(X, Y) :- parent(A, X), parent(B, Y), sibling(A, B).


grandchild(GC, GP) :- grandparent(GP, GC).


num_grandchildren(GP, Count) :-
    findall(GC, grandchild(GC, GP), L),
    sort(L, Unique),
    length(Unique, Count).


ancestor(X, Y) :- parent(X, Y).
ancestor(X, Y) :- parent(X, Z), ancestor(Z, Y).


descendant(X, Y) :- parent(Y, X).
descendant(X, Y) :- parent(Y, Z), descendant(X, Z).


children(Parent, List) :-
    findall(C, parent(Parent, C), L),
    sort(L, List).


siblings(Person, Siblings) :-
    findall(S, (sibling(Person, S)), L),
    sort(L, Siblings).


grandparents(Person, List) :-
    findall(GP, grandparent(GP, Person), L),
    sort(L, List).


all_ancestors(Person, List) :-
    findall(A, ancestor(A, Person), L),
    sort(L, List).


all_descendants(Person, List) :-
    findall(D, descendant(D, Person), L),
    sort(L, List).


