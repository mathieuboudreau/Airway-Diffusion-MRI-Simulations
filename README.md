# Gas Diffusion Simulation in a Budded Cylinder Airway Model of Emphysema
This software simulates diffusion of hyperpolarized gases (e.g. <sup>129</sup>Xe) in a budded cylinder airway model using a
finite difference method.

Switching between healthy and diseased airway values is intended to be used for investigating the behaviour of MRI
measurements between normal lungs and and emphysema.

## Requirements

* GCC

* CMake

* GNU Scientific Library (GSL)

* Check unit testing framework

On a Mac with Brew installed, the following commands successfully installed all 
dependencies on my MacBook Pro (macOS Sierra Version 10.12.4):

```
brew tap homebrew/science
brew install gsl
brew install hdf5
brew install check
```

Note that GCC and CMake were already installed on my machine (possibly through XCode & 
XCode Command Line Tools which are installed seperately).

# Installation

* Generate make files: `cmake .`

* Run make file: `make`

## Tests

From the command line and the repo directory, run `./bin/unit_tests ` to ensure that the 
code was installed successfully (all tests should pass).

## Model

Budded cylinder model details.

Healthy                                             | Diseased
:--------------------------------------------------:|:-----------------------------------------------------:
![Budded Cylinder (healthy)](/figures/healthy.png)  |  ![Budded Cylinder (diseased)](/figures/disease.png)


## About me

**Mathieu Boudreau** is a PhD Candidate at McGill University in the Department of Biomedical Engineering.
He holds a BSc in Physics from the Universite de Moncton ('09), and a MSc in Physics from the University
of Western Ontario ('11).