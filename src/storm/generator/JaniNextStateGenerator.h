#pragma once

#include <boost/container/flat_set.hpp>

#include "storm/generator/NextStateGenerator.h"
#include "storm/generator/TransientVariableInformation.h"

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/ArrayEliminator.h"
#include "storm/storage/jani/OrderedAssignments.h"

namespace storm {
    namespace builder {
        namespace jit {
            template <typename StateType, typename ValueType>
            class Distribution;
        }
    }
    
    namespace jani {
        class Edge;
        class EdgeDestination;
    }

    namespace generator {
        
        template<typename ValueType, typename StateType = uint32_t>
        class JaniNextStateGenerator : public NextStateGenerator<ValueType, StateType> {
        public:
            typedef typename NextStateGenerator<ValueType, StateType>::StateToIdCallback StateToIdCallback;
            typedef boost::container::flat_set<uint_fast64_t> EdgeIndexSet;
            
            JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options = NextStateGeneratorOptions());
            
            virtual ModelType getModelType() const override;
            virtual bool isDeterministicModel() const override;
            virtual bool isDiscreteTimeModel() const override;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) override;
            
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) override;
            
            virtual std::size_t getNumberOfRewardModels() const override;
            virtual storm::builder::RewardModelInformation getRewardModelInformation(uint64_t const& index) const override;
                        
            virtual storm::models::sparse::StateLabeling label(storm::storage::sparse::StateStorage<StateType> const& stateStorage, std::vector<StateType> const& initialStateIndices = {}, std::vector<StateType> const& deadlockStateIndices = {}) override;
            
            virtual std::shared_ptr<storm::storage::sparse::ChoiceOrigins> generateChoiceOrigins(std::vector<boost::any>& dataForChoiceOrigins) const override;
            
        private:
            /*!
             * Retrieves the location index from the given state.
             */
            uint64_t getLocation(CompressedState const& state, LocationVariableInformation const& locationVariable) const;
            
            /*!
             * Sets the location index from the given state.
             */
            void setLocation(CompressedState& state, LocationVariableInformation const& locationVariable, uint64_t locationIndex) const;
            
            /*!
             * Retrieves the tuple of locations of the given state.
             */
            std::vector<uint64_t> getLocations(CompressedState const& state) const;
            
            /*!
             * A delegate constructor that is used to preprocess the model before the constructor of the superclass is
             * being called. The last argument is only present to distinguish the signature of this constructor from the
             * public one.
             */
            JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options, bool flag);
            
            /*!
             * Applies an update to the state currently loaded into the evaluator and applies the resulting values to
             * the given compressed state.
             * @params state The state to which to apply the new values.
             * @params destination The update to apply.
             * @params locationVariable The location variable that is being updated.
             * @params assignmentLevel The assignmentLevel that is to be considered for the update.
             * @return The resulting state.
             */
            CompressedState applyUpdate(CompressedState const& state, storm::jani::EdgeDestination const& destination, storm::generator::LocationVariableInformation const& locationVariable, int64_t assignmentlevel, storm::expressions::ExpressionEvaluator<ValueType> const& expressionEvaluator);
            
            /*!
             * Applies an update to the state currently loaded into the evaluator and applies the resulting values to
             * the given compressed state.
             * @params state The state to which to apply the new values.
             * @params destination The update to apply.
             * @params locationVariable The location variable that is being updated.
             * @params assignmentLevel The assignmentLevel that is to be considered for the update.
             * @return The resulting state.
             */
            void applyTransientUpdate(TransientVariableValuation<ValueType>& transientValuation, storm::jani::EdgeDestination const& destination, int64_t assignmentlevel, storm::expressions::ExpressionEvaluator<ValueType> const& expressionEvaluator);
            
            /*!
             * Retrieves all choices possible from the given state.
             *
             * @param locations The current locations of all automata.
             * @param state The state for which to retrieve the silent choices.
             * @return The action choices of the state.
             */
            std::vector<Choice<ValueType>> getActionChoices(std::vector<uint64_t> const& locations, CompressedState const& state, StateToIdCallback stateToIdCallback);
            
            /*!
             * Retrieves the choice generated by the given edge.
             */
            Choice<ValueType> expandNonSynchronizingEdge(storm::jani::Edge const& edge, uint64_t outputActionIndex, uint64_t automatonIndex, CompressedState const& state, StateToIdCallback stateToIdCallback);
            
            typedef std::vector<std::pair<uint64_t, storm::jani::Edge const*>> EdgeSetWithIndices;
            typedef std::unordered_map<uint64_t, EdgeSetWithIndices> LocationsAndEdges;
            typedef std::vector<std::pair<uint64_t, LocationsAndEdges>> AutomataAndEdges;
            typedef std::pair<boost::optional<uint64_t>, AutomataAndEdges> OutputAndEdges;

            typedef std::pair<uint64_t, EdgeSetWithIndices> AutomatonAndEdgeSet;
            typedef std::vector<AutomatonAndEdgeSet> AutomataEdgeSets;
            
            std::vector<Choice<ValueType>> expandSynchronizingEdgeCombination(AutomataEdgeSets const& edgeCombination, uint64_t outputActionIndex, CompressedState const& state, StateToIdCallback stateToIdCallback);
            void generateSynchronizedDistribution(storm::storage::BitVector const& state, ValueType const& probability, uint64_t position, AutomataEdgeSets const& edgeCombination, std::vector<EdgeSetWithIndices::const_iterator> const& iteratorList, storm::builder::jit::Distribution<StateType, ValueType>& distribution, std::vector<ValueType>& stateActionRewards, EdgeIndexSet& edgeIndices, StateToIdCallback stateToIdCallback);
            void generateSynchronizedDistribution(storm::storage::BitVector const& state, AutomataEdgeSets const& edgeCombination, std::vector<EdgeSetWithIndices::const_iterator> const& iteratorList, storm::builder::jit::Distribution<StateType, ValueType>& distribution, std::vector<ValueType>& stateActionRewards, EdgeIndexSet& edgeIndices, StateToIdCallback stateToIdCallback);

            /*!
             * Checks the list of enabled edges for multiple synchronized writes to the same global variable.
             */
            void checkGlobalVariableWritesValid(AutomataEdgeSets const& enabledEdges) const;
            
            /*!
             * Treats the given transient assignments by calling the callback function whenever a transient assignment
             * to one of the reward variables of this generator is performed.
             */
            void performTransientAssignments(storm::jani::detail::ConstAssignments const& transientAssignments, storm::expressions::ExpressionEvaluator<ValueType> const& expressionEvaluator, std::function<void (ValueType const&)> const& callback);
            
            /*!
             * Builds the information structs for the reward models.
             */
            void buildRewardModelInformation();
            
            /*!
             * Creates the internal information about synchronizing edges.
             */
            void createSynchronizationInformation();
            
            /*!
             * Checks the underlying model for validity for this next-state generator.
             */
            void checkValid() const;
                        
            /// The model used for the generation of next states.
            storm::jani::Model model;
            
            /// The automata that are put into parallel by this generator.
            std::vector<std::reference_wrapper<storm::jani::Automaton const>> parallelAutomata;
            
            /// The vector storing the edges that need to be explored (synchronously or asynchronously).
            std::vector<OutputAndEdges> edges;
            
            /// The transient variables of reward models that need to be considered.
            std::vector<storm::expressions::Variable> rewardVariables;
            
            /// A vector storing information about the corresponding reward models (variables).
            std::vector<storm::builder::RewardModelInformation> rewardModelInformation;
            
            /// A flag that stores whether at least one of the selected reward models has state-action rewards.
            bool hasStateActionRewards;
            
            /// Data from eliminated array expressions. These are required to keep references to array variables in LValues alive.
            storm::jani::ArrayEliminatorData arrayEliminatorData;
            
            /// Information about the transient variables of the model.
            TransientVariableInformation<ValueType> transientVariableInformation;
        };
        
    }
}
