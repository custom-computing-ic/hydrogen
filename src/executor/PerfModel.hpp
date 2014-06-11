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
        int LoadFromDisk(PerfModel& pm);
        Implementation* getImp() { return imp;}
        friend std::ostream& operator<<(std::ostream& os, const PerfModel& pm) {
          double projectedCost = 0.0;
          std::vector<double> input_data;
//          os << std::setprecision(3);
          for (int i = 0; i <= pm.order; i++) {
            input_data.push_back(pow(2.0,i));
          }
          for (int i = 0; i < pm.numberOfFeatures; i++) {
            os << pm.weights[i];
            if ( i > 0)
              os  << "x^" << i;
            if (i < pm.numberOfFeatures -1)
              os << " + ";
            projectedCost += pm.weights[i] * input_data[i];
          }
          os << "\t = " << projectedCost << "\t(x=" << 2.0 << ")\n";
          return os;
        }
        friend bool operator== (const PerfModel& lhs, const PerfModel& rhs) {
          bool result = true;
          result &= lhs.alpha == rhs.alpha;
          result &= lhs.iterations == rhs.iterations;
          result &= lhs.numberOfFeatures == rhs.numberOfFeatures;
          result &= lhs.numberOfDataPoints == rhs.numberOfDataPoints;
          result &= lhs.order == rhs.order;
          result &= lhs.features == rhs.features;
          result &= lhs.weights == rhs.weights; 
          result &= lhs.milliseconds == rhs.milliseconds;
          return result;
        }
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
