#ifndef __TRAINING_ALGORITHM_H__
#define __TRAINING_ALGORITHM_H__
#include <vector>

class NeuralNetwork;
class TrainingAlgorithm {
protected:
    NeuralNetwork* network;
    float alpha;
public:
    virtual void train(std::vector<std::vector<float>> trainData, std::vector<std::vector<float>> trainLabels) = 0;
    virtual float test(std::vector<std::vector<float>> testData, std::vector<std::vector<float>> testLabels) = 0;
};

#endif