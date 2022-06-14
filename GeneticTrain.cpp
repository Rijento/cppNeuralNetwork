#include "GeneticTrain.hpp"
#include "NeuralNetwork.hpp"
#include "Layer.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include <math.h>
#include <thread>
#include <random>


// Helper Functions
bool tupleComparer::operator() (const std::tuple<NeuralNetwork*, float> x, const std::tuple<NeuralNetwork*, float> y) {
    return std::get<1>(x) >= std::get<1>(y);
}

// Private Class Functions
template <class T>
NeuralNetwork* GeneticTrain<T>::procreate(NeuralNetwork* dominant, NeuralNetwork* submissive) {
    srand(static_cast <unsigned> (time(NULL)));
    NeuralNetwork* ofspring = dominant->clone();
    uint8_t neuronCount = ofspring->getHiddenNeuronCount() + ofspring->getInputLayer()->getNeurons()->size();
    int j = 0;
    for (int i = 0; i < ofspring->getHiddenLayers()->size() + 1; i++) {
        NeuronIterator it;
        NeuronIterator endIt;
        Layer* slay; // Can't rely on second iterator
        if (i == 0) { // input layer
            it = ofspring->getInputLayer()->getNeurons()->begin();
            slay = submissive->getInputLayer();
            endIt = ofspring->getInputLayer()->getNeurons()->end();
        } else {
            LayerIterator lit = ofspring->getHiddenLayers()->begin();
            LayerIterator slit = submissive->getHiddenLayers()->begin();
            assert((*lit)->getLayerDepth() == (*slit)->getLayerDepth());
            std::advance(lit, i-1);
            std::advance(slit, i-1);
            it = (*lit)->getNeurons()->begin();
            slay = (*slit);
            endIt = (*lit)->getNeurons()->end();
        }

        bool useSub = false;
        for (; it != endIt; ++it, ++j) {
            float roll1 = ((float) rand() / (RAND_MAX));
            float roll2 = ((float) rand() / (RAND_MAX));
            float roll3 = randWeight();
            std::string key = (*it).first;
            std::unordered_set<Synapse*, synapseHash, synapseCompare> ssyns = *(slay->getNeuronById(key)->getSynapses());
            for(Synapse* synapse : *((*it).second->getSynapses())) {
                if (roll1 <= mutation) {
                    synapse->setWeight(roll3);
                } else if (roll2 > crossover) { // Already technically crossed over (we want to keep as much of the dominant as possible)
                    Synapse* ssyn = *(ssyns.find(synapse));
                    synapse->setWeight(ssyn->getWeight());
                }
            }
        }
    }
    return ofspring;
}

template <class T>
void GeneticTrain<T>::trainThread(
    std::vector<std::tuple<NeuralNetwork*,float>>& subGeneration,
    std::set<T>& trainData,
    std::function<float(NeuralNetwork*,T&)> calculateScore
    ) {
    for (std::tuple<NeuralNetwork*,float> pair : subGeneration) {
        for (T point : trainData) {
            float adjustment = calculateScore(std::get<0>(pair), point);
            std::get<1>(pair) += adjustment;
            if ( adjustment <= 0 ) {
                break;
            }
        }
    }
}

template <class T>
void GeneticTrain<T>::geneticShuffle() {
    std::set<std::tuple<NeuralNetwork*, float>, tupleComparer> nextGeneration;

    while (currentGeneration.size() > 10) {
        GenerationIterator it = currentGeneration.begin();
        std::advance(it, 10);
        std::tuple<NeuralNetwork*, float> pair = currentGeneration.extract(it);
        NeuralNetwork* net = std::get<0>(pair);
        delete net;
    }

    // Do Genetic Stuff

    // 10 completely random
    for (int i = 0; i < 10; i++) {
        std::tuple<NeuralNetwork*, float> pair = *currentGeneration.begin();
        NeuralNetwork* net = std::get<0>(pair);
        NeuralNetwork* randomNet = net->clone();
        randomNet->randomizeWeights();
        std::tuple<NeuralNetwork*,float> toInsert = std::make_tuple<NeuralNetwork*,float>(randomNet, 0.0);
        nextGeneration.insert(toInsert);
    }

    // 35 ofspring of the highest scoring net & next highest scoring nets
    for (int i = 0; i < 10; i++) {
        GenerationIterator dit = currentGeneration.begin();
        std::advance(dit, i);
        NeuralNetwork* dominant = std::get<0>(*dit);
        for (int j = 0; j < 10 - i; j++) {
            GenerationIterator sit = currentGeneration.begin();
            std::advance(sit, j);
            if (sit != dit) {
                NeuralNetwork* submissive = std::get<0>(*sit);
                NeuralNetwork* ofspring = procreate(dominant, submissive);
                std::tuple<NeuralNetwork*,float> toInsert = std::make_tuple<NeuralNetwork*,float>(ofspring, 0.0);
                nextGeneration.insert(toInsert);
            }
        }

    }

    // Current Top 5
    for(int i = 0; i < 5; i++) {
        std::tuple<NeuralNetwork*, float> pair = currentGeneration.extract(currentGeneration.begin());
        std::get<1>(pair) = 0.0;
        nextGeneration.insert(pair);
    }

    // Swap the sets
    currentGeneration.swap(nextGeneration);

    // Delete anything still in next generation
    while (!nextGeneration.empty()) {
        std::tuple<NeuralNetwork*, float> pair = nextGeneration.extract(nextGeneration.begin());
        NeuralNetwork* net = std::get<0>(pair);
        delete net;
    }
}


// Public Class Functions
template <class T>
GeneticTrain<T>::GeneticTrain(
    NeuralNetwork* archetype,
    float crossoverIn,
    float mutationIn
    ) {
    srand(static_cast <unsigned> (time(0)));
    crossover = crossoverIn;
    mutation = mutationIn;

    for (int i = 0; i < 49; i++) {
        NeuralNetwork* net = archetype->clone();
        net->randomizeWeights();
        std::tuple<NeuralNetwork*,float> toInsert = std::make_tuple<NeuralNetwork*,float>(net, 0.0);
        currentGeneration.insert(toInsert);
    }
    currentGeneration.insert(archetype);
}

template <class T>
void GeneticTrain<T>::train(
    std::set<T>& trainData,
    std::function<float(NeuralNetwork*,T&)> calculateScore
    ) {
    // Split generation into 5 threads with 3 bots each;
    std::vector<std::vector<std::tuple<NeuralNetwork*, float>>> threadTuples = {{},{},{},{},{}};
    // 0,5,10,15,20,25,30,35,40,45
    // 1,6,11,16,21,26,31,36,41,46
    // 2,7,12,17,22,27,32,37,42,47
    // 3,8,13,18,23,28,33,38,43,48
    // 4,9,14,19,24,29,34,39,44,49
    for (int i = 0; i < 10; i++) {
        threadTuples[0].push_back(std::move(currentGeneration.extract(currentGeneration.begin()).value()));
        threadTuples[1].push_back(std::move(currentGeneration.extract(currentGeneration.begin()).value()));
        threadTuples[2].push_back(std::move(currentGeneration.extract(currentGeneration.begin()).value()));
        threadTuples[3].push_back(std::move(currentGeneration.extract(currentGeneration.begin()).value()));
        threadTuples[4].push_back(std::move(currentGeneration.extract(currentGeneration.begin()).value()));
    }

    // Spawn 5 threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        // The "this" in the next function had me pulling my hair out.
        // This is what I get for not formally learning how to do threads, isn't it...
        threads.emplace_back(std::thread(&GeneticTrain::trainThread, this, std::ref(threadTuples[i]), std::ref(trainData), calculateScore));
    }
    for (std::thread& th : threads) {
        th.join();
    }

    // order the bots by bank balance
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            currentGeneration.insert(threadTuples[i].back());
            threadTuples[i].pop_back();
        }
    } // Natural set sorting should take care of things

    geneticShuffle(startingData);

}

template <class T>
NeuralNetwork* GeneticTrain<T>::getBestPerformer() {
    return std::get<0>(*currentGeneration.begin());
}

template <class T>
GeneticTrain<T>::~GeneticTrain() {
    while (!currentGeneration.empty()) {
        std::tuple<NeuralNetwork*, float> pair = currentGeneration.extract(currentGeneration.begin());
        NeuralNetwork* net = std::get<0>(pair);
        delete net;
    }
}