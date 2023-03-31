//
//  DFA.hpp
//

#ifndef DFA1_hpp
#define DFA1_hpp

#include <stdio.h>
#include <string>
#include <vector>

using std::vector;

class DFA1 {
public:
    float push(vector<int>& rrs);
    float _compute();
    void _initialize();
    vector<float> _scales(double a, double b, int k);
};

#endif /* DFA1 */
