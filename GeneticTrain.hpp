#ifndef __GENETIC_TRAIN_H__
#define __GENETIC_TRAIN_H__

#include "TrainingAlgorithm.hpp"
#include <functional>
#include <set>
#include <vector>

#define GenerationIterator std::set<std::tuple<NeuralNetwork*,float>>::iterator
#define GenerationRIterator std::set<std::tuple<NeuralNetwork*,float>>::reverse_iterator

class Synapse;
class NeuralNetwork;

struct tupleComparer {
    bool operator() (const std::tuple<NeuralNetwork*, float> x, const std::tuple<NeuralNetwork*, float> y);
};

template <class T>
class GeneticTrain {
private:
    std::set<std::tuple<NeuralNetwork*, float>, tupleComparer> currentGeneration;
    NeuralNetwork* procreate(NeuralNetwork* dominant, NeuralNetwork* submissive);
    void trainThread(
        std::vector<std::tuple<NeuralNetwork*,float>>& subGeneration,
        std::set<T>& trainData,
        std::function<float(NeuralNetwork*,T&)> calculateScore
        );
    void geneticShuffle();
    float crossover;
    float mutation;
public:
    GeneticTrain(
        NeuralNetwork* archetype,
        float crossoverIn,
        float mutationIn);
    void train(std::set<T>& trainData, std::function<float(NeuralNetwork*,T&)> calculateScore);
    float test(std::set<T>& testData);
    NeuralNetwork* getBestPerformer();
    ~GeneticTrain();
};

#endif