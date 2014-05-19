#ifndef _MODEL_H_
#define _MODEL_H_
#include <map>
#include <functional>
#include <vector>
#include <cmath>
#include <Implementation.hpp> 
class PerfModel {
    private:
        void LinearRegression();
        double alpha;
        uint64_t iterations;
        int numberOfFeatures;
        int numberOfDataPoints;
        int order;
        std::vector<std::vector<double>> features;
        std::vector<double> weights; 
    public:
        PerfModel(Implementation*);
        PerfModel(Implementation *param, double alpha, uint64_t it, int feat, int order);
        double QueryModel(double);
        int CreateModel(int, std::function<double(int)>);

        Implementation *imp;
        std::vector<double> milliseconds;
};
#endif
