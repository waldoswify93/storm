#include "src/modelchecker/CheckSettings.h"

#include "src/logic/Formulas.h"

#include "src/utility/constants.h"

namespace storm {
    namespace modelchecker {

        template<typename ValueType>
        CheckSettings<ValueType>::CheckSettings() : CheckSettings(boost::none, boost::none, false, boost::none, false, false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        CheckSettings<ValueType>::CheckSettings(boost::optional<storm::OptimizationDirection> const& optimizationDirection, boost::optional<std::string> const& rewardModel, bool onlyInitialStatesRelevant, boost::optional<std::pair<storm::logic::ComparisonType, ValueType>> const& initialStatesBound, bool qualitative, bool produceStrategies) : optimizationDirection(optimizationDirection), rewardModel(rewardModel), onlyInitialStatesRelevant(onlyInitialStatesRelevant), initialStatesBound(initialStatesBound), qualitative(qualitative), produceStrategies(produceStrategies) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        CheckSettings<ValueType> CheckSettings<ValueType>::fromToplevelFormula(storm::logic::Formula const& formula) {
            return fromFormula(formula, true);
        }
        
        template<typename ValueType>
        CheckSettings<ValueType> CheckSettings<ValueType>::fromNestedFormula(storm::logic::Formula const& formula) {
            return fromFormula(formula, false);
        }
        
        template<typename ValueType>
        CheckSettings<ValueType> CheckSettings<ValueType>::fromFormula(storm::logic::Formula const& formula, bool toplevel) {
            boost::optional<storm::OptimizationDirection> optimizationDirection;
            boost::optional<std::string> rewardModel;
            boost::optional<std::pair<storm::logic::ComparisonType, ValueType>> initialStatesBound;
            bool qualitative = false;
            bool onlyInitialStatesRelevant = !toplevel;
            bool produceStrategies = false;
            
            if (formula.isProbabilityOperatorFormula()) {
                storm::logic::ProbabilityOperatorFormula const& probabilityOperatorFormula = formula.asProbabilityOperatorFormula();
                if (probabilityOperatorFormula.hasOptimalityType()) {
                    optimizationDirection = probabilityOperatorFormula.getOptimalityType();
                }
                
                if (probabilityOperatorFormula.hasBound()) {
                    if (onlyInitialStatesRelevant) {
                        initialStatesBound = std::make_pair(probabilityOperatorFormula.getComparisonType(), static_cast<ValueType>(probabilityOperatorFormula.getBound()));
                    }
                    if (probabilityOperatorFormula.getBound() == storm::utility::zero<ValueType>() || probabilityOperatorFormula.getBound() == storm::utility::one<ValueType>()) {
                        qualitative = true;
                    }
                    if (!optimizationDirection) {
                        optimizationDirection = probabilityOperatorFormula.getComparisonType() == storm::logic::ComparisonType::Less || probabilityOperatorFormula.getComparisonType() == storm::logic::ComparisonType::LessEqual ? OptimizationDirection::Maximize : OptimizationDirection::Minimize;
                    }
                }
            } else if (formula.isRewardOperatorFormula()) {
                storm::logic::RewardOperatorFormula const& rewardOperatorFormula = formula.asRewardOperatorFormula();
                rewardModel = rewardOperatorFormula.getOptionalRewardModelName();

                if (rewardOperatorFormula.hasOptimalityType()) {
                    optimizationDirection = rewardOperatorFormula.getOptimalityType();
                }
                
                if (rewardOperatorFormula.hasBound()) {
                    if (onlyInitialStatesRelevant) {
                        initialStatesBound = std::make_pair(rewardOperatorFormula.getComparisonType(), static_cast<ValueType>(rewardOperatorFormula.getBound()));
                    }
                    if (rewardOperatorFormula.getBound() == storm::utility::zero<ValueType>()) {
                        qualitative = true;
                    }
                    if (!optimizationDirection) {
                        optimizationDirection = rewardOperatorFormula.getComparisonType() == storm::logic::ComparisonType::Less || rewardOperatorFormula.getComparisonType() == storm::logic::ComparisonType::LessEqual ? OptimizationDirection::Maximize : OptimizationDirection::Minimize;
                    }
                }
            }
            return CheckSettings<ValueType>(optimizationDirection, rewardModel, onlyInitialStatesRelevant, initialStatesBound, qualitative, produceStrategies);
        }
        
        template<typename ValueType>
        bool CheckSettings<ValueType>::isOptimizationDirectionSet() const {
            return static_cast<bool>(optimizationDirection);
        }
        
        template<typename ValueType>
        storm::OptimizationDirection const& CheckSettings<ValueType>::getOptimizationDirection() const {
            return optimizationDirection.get();
        }
        
        template<typename ValueType>
        bool CheckSettings<ValueType>::isRewardModelSet() const {
            return static_cast<bool>(rewardModel);
        }
        
        template<typename ValueType>
        std::string const& CheckSettings<ValueType>::getRewardModel() const {
            return rewardModel.get();
        }
        
        template<typename ValueType>
        bool CheckSettings<ValueType>::isOnlyInitialStatesRelevantSet() const {
            return onlyInitialStatesRelevant;
        }
        
        template<typename ValueType>
        bool CheckSettings<ValueType>::isInitialStatesBoundSet() const {
            return static_cast<bool>(initialStatesBound);
        }
        
        template<typename ValueType>
        std::pair<storm::logic::ComparisonType, ValueType> const& CheckSettings<ValueType>::getInitialStatesBound() const {
            return initialStatesBound.get();
        }
        
        template<typename ValueType>
        bool CheckSettings<ValueType>::isQualitativeSet() const {
            return qualitative;
        }
        
        template<typename ValueType>
        bool CheckSettings<ValueType>::isProduceStrategiesSet() const {
            return produceStrategies;
        }
        
        template class CheckSettings<double>;
        
    }
}