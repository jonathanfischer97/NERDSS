#include "math/constants.hpp"
#include "math/math_functions.hpp"
#include "math/rand_gsl.hpp"
#include "reactions/unimolecular/unimolecular_reactions.hpp"
#include "tracing.hpp"


void check_for_zeroth_order_creation(unsigned simItr, Parameters &params, SimulVolume &simulVolume, const std::vector<ForwardRxn> &forwardRxns,
                                     const std::vector<CreateDestructRxn> &createDestructRxns, std::vector<Molecule> &moleculeList,
                                     std::vector<Complex> &complexList, std::vector<MolTemplate> &molTemplateList,
                                     std::map<std::string, int> &observablesList, copyCounters &counterArrays, Membrane &membraneObject)
{
    // TRACE();
    for (auto &oneRxn : createDestructRxns)
    {
        if (oneRxn.rxnType == ReactionType::zerothOrderCreation)
        {
            MolTemplate &oneTemp = molTemplateList[oneRxn.productMolList.at(0).molTypeIndex];
            double bigNum{1E200};
            if (oneTemp.isImplicitLipid == true)
            {
                int indexIlState = oneRxn.productMolList.at(0).interfaceList.at(0).absIfaceIndex;
                // Calculate the probability
                // Poisson process, p(zero events) = exp(-k * V * deltaT), where k = rate (M/s), V = volume (L), deltaT=
                // timestep (s)
                // long double lambda { Constants::avogadro * oneRxn.rateList[0].rate * membraneObject.waterBox.volume
                //     * Constants::nm3ToLiters * params.timeStep * Constants::usToSeconds };
                double Volume = 0.0;
                // Change Volume depending upon box or sphere simulation
                if (membraneObject.isSphere)
                {
                    Volume = membraneObject.sphereVol;
                }
                else if (membraneObject.hasCompartment == true && oneTemp.insideCompartment == true)
                {
                    //double r = membraneObject.compartmentR;
                    double Vcompartment = 4.0 / 3.0 * M_PI * pow(membraneObject.compartmentR, 3.0);
                    Volume = Vcompartment;
                }
                else if (membraneObject.hasCompartment == true && oneTemp.outsideCompartment == true)
                {
                    //double r = membraneObject.compartmentR;
                    double Vcompartment = 4.0 / 3.0 * M_PI * pow(membraneObject.compartmentR, 3.0);
                    Volume = membraneObject.waterBox.volume - Vcompartment;
                }
                else
                {
                    Volume = membraneObject.waterBox.volume;
                }
                long double lambda{Constants::avogadro * oneRxn.rateList[0].rate * Volume * Constants::nm3ToLiters * params.timeStep * Constants::usToSeconds};
                long double prob{exp(-lambda)};
                unsigned numEvents{0};
                double rNum{rand_gsl()};
                // recursive parameter
                double factor = 1;
                double lampow = lambda;
                double explam = exp(-lambda);
                // double rNum2 { rNum + rand_gsl() * Constants::iRandMax }; // to get higher resolution

                /*
                while (rNum > prob) {
                    ++numEvents;
                    prob += (exp(-lambda) * pow(lambda, numEvents)) / MathFuncs::gammFactorial(numEvents);
                }
                */

                // recursive poisson
               
                while (rNum > prob)
                {
                    ++numEvents;
                    // prob += (exp(-lambda) * pow(lambda, numEvents)) / MathFuncs::gammFactorial(numEvents);
                    // this method is much faster, same accuracy
                    prob += explam * lampow / factor;
                    lampow = lampow * lambda;  // accumulate the powers
                    factor *= (numEvents + 1); // accumulate the factorial
                }
                if(factor>bigNum){
                    std::cout<<"factor too large"<< std::endl;
                    numEvents = gsl_ran_poisson(r, lambda);
                }
                // if (numEvents > 0)
                // {
                //     // std::cout << "Creating " << numEvents << " molecule(s) of type " << oneTemp.molName << " at iteration "
                //     //           << simItr << '\n';
                // }

                if (oneRxn.isObserved)
                {
                    auto observeItr = observablesList.find(oneRxn.observeLabel);
                    if (observeItr == observablesList.end())
                    {
                        // std::cerr << "WARNING: Observable " << oneRxn.observeLabel << " not defined.\n";
                    }
                    else
                    {
                        observeItr->second += numEvents;
                    }
                }

                while (numEvents > 0)
                {
                    ++counterArrays.copyNumSpecies[indexIlState];
                    ++membraneObject.numberOfFreeLipidsEachState[indexIlState];
                    --numEvents;
                }
            }
            else
            {
                // Calculate the probability
                // Poisson process, p(zero events) = exp(-k * V * deltaT), where k = rate (M/s), V = volume (L), deltaT=
                // timestep (s)
                double Volume = 0.0;
                // Change Volume depending upon box or sphere simulation
                if (membraneObject.isSphere)
                {
                    Volume = membraneObject.sphereVol;
                }
                else if (membraneObject.hasCompartment == true && oneTemp.insideCompartment == true)
                {
                    //double r = membraneObject.compartmentR;
                    double Vcompartment = 4.0 / 3.0 * M_PI * pow(membraneObject.compartmentR, 3.0);
                    Volume = Vcompartment;
                }
                else if (membraneObject.hasCompartment == true && oneTemp.outsideCompartment == true)
                {
                    //double r = membraneObject.compartmentR;
                    double Vcompartment = 4.0 / 3.0 * M_PI * pow(membraneObject.compartmentR, 3.0);
                    Volume = membraneObject.waterBox.volume - Vcompartment;
                }
                else
                {
                    Volume = membraneObject.waterBox.volume;
                }
                long double lambda{Constants::avogadro * oneRxn.rateList[0].rate * Volume * Constants::nm3ToLiters * params.timeStep * Constants::usToSeconds};
                long double prob{exp(-lambda)};
                int numEvents{0};
                double rNum{rand_gsl()};
                // recursive parameter
                double factor = 1;
                double lampow = lambda;
                double explam = exp(-lambda);
                // double rNum2 { rNum + rand_gsl() * Constants::iRandMax }; // to get higher resolution

                // std::cout << "Volume: " << std::setprecision(20) << Volume << std::endl;
                // std::cout << "create prob: " << std::setprecision(20) << prob << std::endl;
                // exit(1);

                while (rNum > prob)
                {
                    ++numEvents;
                    // prob += (exp(-lambda) * pow(lambda, numEvents)) / MathFuncs::gammFactorial(numEvents);
                    // this method is much faster, same accuracy
                    prob += explam * lampow / factor;
                    lampow = lampow * lambda;  // accumulate the powers
                    factor *= (numEvents + 1); // accumulate the factorial
                }
                if(factor>bigNum){
                    std::cout<<"factor too large"<< std::endl;
                    numEvents = gsl_ran_poisson(r, lambda);
                }
                // whether to use the strong titration!
                // This is specific for titration in and out of compartment
                if (oneRxn.rateList[0].rate == -1)
                { //} && membraneObject.hasCompartment == true ){
                    numEvents = oneTemp.copies - oneTemp.monomerList.size();
                    if (numEvents < 0) // for the monomers more than the target number, then no need titration but need destruction
                        numEvents = 0;
                }

                // if (numEvents > 0)
                // {
                //     // std::cout << "Creating " << numEvents << " molecule(s) of type " << oneTemp.molName << " at iteration "
                //     //           << simItr << '\n';
                // }

                if (oneRxn.isObserved)
                {
                    auto observeItr = observablesList.find(oneRxn.observeLabel);
                    if (observeItr == observablesList.end())
                    {
                        // std::cerr << "WARNING: Observable " << oneRxn.observeLabel << " not defined.\n";
                    }
                    else
                    {
                        observeItr->second += numEvents;
                    }
                }

                while (numEvents > 0)
                {
                    // these aren't used here, just for the function
                    int newMolIndex{0};
                    int newComIndex{0};

                    create_molecule_and_complex_from_rxn(0, newMolIndex, newComIndex, false, oneTemp, params, oneRxn, simulVolume, moleculeList, complexList, molTemplateList, forwardRxns, membraneObject);

                    moleculeList[newMolIndex].isGhosted = false;
                    moleculeList[newMolIndex].id = Molecule::maxID++;
                    moleculeList[newMolIndex].need_to_send = true;

                    // update the copy number arrays
                    for (const auto &iface : moleculeList[newMolIndex].interfaceList)
                        ++counterArrays.copyNumSpecies[iface.index];

                    --numEvents;
                }
            }
        }
    }
}
