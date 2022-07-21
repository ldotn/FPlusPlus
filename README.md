# FPlusPlus
Header only library to make **Modern C++** more functional (no pun intended).
It add things from functional programming that act mostly as syntactic sugar, trying to have zero to none runtime overhead. The idea is to bring some of the nice things of Python into C++

### Things 
* Zip (for 2 variables)
* Enumerate

For now the functions can't be concatenated, that is, you can't call *enumerate(zip(a,b))*

### Stuff
It now has some SIMD vector functions and image manipulation stuff, not because it's related but because I needed a place to put them