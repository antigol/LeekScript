function is_perm(a, b) {
    var sa = String.split('' + a, '')
    var sb = String.split('' + b, '')
    if (sa.size() != sb.size()) { return false }
    for var i in sa {
        var p = sb.search(i, 0)
        if (p == -1) { return false }
        Array.remove(sb, p)
    }
    return true
}

is_perm(1, 1)
//is_perm(1234567, 7461253)
