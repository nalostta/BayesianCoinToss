# BayesianCoinToss
## Introduction
 
This is a simple program to test and understand the concepts of likelihood, prior, & posterior probabilities.

Consider a coin which could either be biased, or unbiased.
 - For an unbiased coin, the probabilities of head & tails is (Puh=0.5, Put=0.5) 
 - For a  biased it is set to a fixed map of (Pbh,Pbt), which remains constant.
 - The application does not know whether the coin is biased or not.
 - It's job is to guess what is the likelihood that the coin is un/biased given a sequence of data points at every turn.
 - With every input, you give the outcome of the coin toss to the application program, and it updates it's estimation based on this new data.

## How To Build
 > mkdir build
 
 > cd build
 
 > cmake ..
  
 To play the game manually, run:
 > cmake --build . --target bct
 > ./bct 

 To run the game over a predetermined sequence of inputs and plot confidence on a graph, update TestInput in bct_config.json and build test app:
 > cmake --build . --target test
 > ./test 

 Note: You will need gnuplot for this.

## Experiment Setup
### init params:
 1. Biased coin probability map
 2. Initial estimate 

### Running experiment:
 - Input: coin toss outcome
 - Output: Probability of coin being biased or unbiased.
