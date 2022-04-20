#ifndef __BACKPROPTRAIN_ALGORITHM_H__
#define __BACKPROPTRAIN_ALGORITHM_H__

#include "TrainingAlgorithm.hpp"

class Synapse;

class BackpropagationTrain : public TrainingAlgorithm {
private:
    float MSE(float, float);
    float backpropagate(Synapse*, std::vector<float>&, std::vector<float>&);
public:
    BackpropagationTrain(NeuralNetwork* networkIn, float alphaIn);
    void train(std::vector<std::vector<float>> trainData, std::vector<std::vector<float>> trainLabels) override;
    float test(std::vector<std::vector<float>> testData, std::vector<std::vector<float>> testLabels) override;
};

#endif