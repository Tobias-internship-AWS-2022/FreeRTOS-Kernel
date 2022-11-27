#ifndef VERIFAST_LISTS_EXTENDED_H
#define VERIFAST_LISTS_EXTENDED_H

/* This file contains lemmas that would fit `list.gh` which is part
 * of VeriFast's standard library.
 */




// TODO: prove
/*@
lemma void head_drop_n_equals_nths<t>(list<t> xs, int n)
requires n >= 0;
ensures head(drop(n, xs)) == nth(n, xs);
{
    // Will prove later. For now, we only validate with an example. 
    
    list<int> _xs = cons(1, cons(2, cons(3, cons(4, cons(5, cons(6, cons(7, nil)))))));
    int _n = 4;

    list<int> dn = drop(_n, _xs);
    int hdn = head(dn);
    int nthn = nth(_n, _xs);
    
    assert( hdn == head(drop(_n, _xs)) );
    assert( nthn == nth(_n, _xs ));
    assert( head(drop(_n, _xs)) == nth(_n, _xs) );

    
    // ADMIT LEMMA, PROVE LATER
    assume(false);
}

// TODO: prove
lemma void drop_index_equals_singleton_implies_last_element<t>(list<t> xs, t x)
requires drop(index_of(x, xs), xs) == cons(x, nil);
ensures index_of(x, xs) == length(xs) - 1;
{
    // Will prove later. For now, we only validate with an example. 
    list<int> _xs = cons(1, cons(2, cons(3, cons(4, cons(5, cons(6, cons(7, nil)))))));
    int _x = 7;

    int i = index_of(_x, _xs);
    list<int> d = drop(index_of(x, xs), _xs);

    assert( index_of(_x, _xs) == length(_xs) - 1 );

    // ADMIT LEMMA, PROVE LATER
    assume(false);
}

// TODO: prove
// Can we replace this by standard lemma `drop_n_plus_one`?
lemma void drop_cons<t>(list<t> xs, int n)
requires n < length(xs);
ensures drop(n, xs) == cons(nth(n, xs), drop(n+1, xs));
{
    // Will prove later. For now, we only validate with an example. 
    list<int> _xs = cons(1, cons(2, cons(3, cons(4, cons(5, cons(6, cons(7, nil)))))));
    int _n = 3;

    list<int> dn = drop(_n, _xs);
    int nthn = nth(_n, _xs);
    list<int> dnp1 = drop(_n + 1, _xs);

    assert( drop(_n, _xs) == cons(nth(_n, _xs), drop(_n+1, _xs)) );

    // ADMIT LEMMA, PROVE LATER
    assume(false);
}

// TODO: prove
lemma void nth_index<t>(list<t> xs, t x)
requires mem(x, xs) == true;
ensures nth(index_of(x, xs), xs) == x;
{
    // Will prove later. For now, we only validate with an example. 
    list<int> _xs = cons(1, cons(2, cons(3, cons(4, cons(5, cons(6, cons(7, nil)))))));
    int _x = 4;

    int i = index_of(_x, _xs);
    int nthi = nth(index_of(_x, _xs), _xs);

    assert( nth(index_of(_x, _xs), _xs) == _x );

    // ADMIT LEMMA, PROVE LATER
    assume(false);
}

// TODO: prove
lemma void mem_prefix_implies_mem<t>(t x, list<t> xs, int n);
requires mem(x, take(n, xs)) == true;
ensures mem(x, xs) == true;

// TODO: prove
lemma void mem_suffix_implies_mem<t>(t x, list<t> xs, int n);
requires mem(x, drop(n, xs)) == true;
ensures mem(x, xs) == true;

// TODO: Can we prove this in VeriFast or do we have to axiomatise?
lemma void drop_n_plus_m<t>(list<t> xs, int n, int m);
requires true;
ensures drop(n, drop(m, xs)) == drop(n + m, xs);
@*/



#endif /* VERIFAST_LISTS_EXTENDED_H */