!int <64> sum_20(!int <64> a, !int <64> b, !int <64> c, !int <64> d, !int <64> e, !int <64> f, !int <64> g, !int <64> h, !int <64> i, !int <64> j, !int <64> k, !int <64> l, !int <64> m, !int <64> n, !int <64> o, !int <64> p, !int <64> q, !int <64> r, !int <64> s, !int <64> t) 
{
    ret a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p + q + r + s + t;
}

!int <64> main() 
{
    ret sum_20(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1);
}

: expected exit code : 22