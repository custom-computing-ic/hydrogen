#ifndef _MODEL_H_
#define _MODEL_H_
#include <map>
#include <functional>
#include <vector>
#include <cmath>
#include <Implementation.hpp> 
#include <stdexcept>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

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

        std::vector<double> milliseconds;
    public:
        Implementation *imp; // Fix this 
        PerfModel(Implementation*);
        PerfModel(Implementation *param, double alpha, uint64_t it, int feat, int order);
        double QueryModel(double);
        int CreateModel(int, std::function<double(int)>);
        int SaveToDisk(const std::string& fname);
        int SaveToDisk();
        int LoadFromDisk(const std::string& fname);
        int LoadFromDisk();
        template <class Archive> 
        void serialize(Archive& ar, const unsigned int version) {
          if (imp == nullptr) {
            throw std::invalid_argument("Invalid Implementation when attempting to serialize PerfModel\n");
          }
          ar & alpha;
          ar & iterations;
          ar & numberOfFeatures;
          ar & order;
          ar & features;
          ar & weights;
          ar & milliseconds;
          //TODO - Save implementation name?
        }

};
#endif
