#include "storm/storage/jani/JaniScopeChanger.h"

#include <map>
#include <set>
#include <boost/any.hpp>

#include "storm/storage/expressions/Variable.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/traverser/JaniTraverser.h"

namespace storm {
    namespace jani {
        
        namespace detail {
            class VariableAccessedTraverser : public ConstJaniTraverser {
            public:
                VariableAccessedTraverser(std::set<storm::expressions::Variable> const& varSet) : varSet(varSet) {
                    // Intentionally left empty
                }
                using ConstJaniTraverser::traverse;
                
                virtual void traverse(storm::expressions::Expression const& expression, boost::any const& data) override {
                    bool* result = boost::any_cast<bool *>(data);
                    if (*result) { return; }
                    *result = expression.containsVariable(varSet);
                }
                
            private:
                std::set<storm::expressions::Variable> const& varSet;
            };
            
            std::set<uint64_t> getAutomataAccessingVariable(storm::expressions::Variable const& variable, Model const& model) {
                std::set<uint64_t> res;
                for (uint64_t i = 0; i < model.getNumberOfAutomata(); ++i) {
                    if (model.getAutomaton(i).getVariables().hasVariable(variable)) {
                        res.insert(i);
                    } else {
                        VariableAccessedTraverser vat({variable});
                        bool varAccessed = false;
                        vat.traverse(model.getAutomaton(i), &varAccessed);
                        if (varAccessed) {
                            res.insert(i);
                        }
                    }
                }
                return res;
            }
        }
        
        void JaniScopeChanger::makeVariableGlobal(storm::expressions::Variable const& variable, Model& model) const {
            uint64_t automatonIndex = 0;
            for (; automatonIndex < model.getNumberOfAutomata(); ++automatonIndex) {
                if (model.getAutomaton(automatonIndex).getVariables().hasVariable(variable)) {
                    break;
                }
            }
            std::map<Variable const*, std::reference_wrapper<Variable const>> remapping;
            auto oldJaniVar = model.getAutomaton(automatonIndex).getVariables().eraseVariable(variable);
            remapping.emplace(oldJaniVar.get(), model.addVariable(*oldJaniVar));
            
            // Only one automaton accesses this variable
            model.getAutomaton(automatonIndex).changeAssignmentVariables(remapping);
        }
        
        void JaniScopeChanger::makeVariableLocal(storm::expressions::Variable const& variable, Model& model, uint64_t automatonIndex) const {
            std::map<Variable const*, std::reference_wrapper<Variable const>> remapping;
            auto oldJaniVar = model.getGlobalVariables().eraseVariable(variable);
            remapping.emplace(oldJaniVar.get(), model.getAutomaton(automatonIndex).addVariable(*oldJaniVar));
            // Only one automaton accesses this variable (otherwise this call would be illegal)
            model.getAutomaton(automatonIndex).changeAssignmentVariables(remapping);
        }
        
        bool JaniScopeChanger::canMakeVariableGlobal(storm::expressions::Variable const& variable, Model const& model) const {
            if (model.hasGlobalVariable(variable.getName())) {
                return false;
            }
            // Check whether there are multiple local variables with this name
            bool foundVar = false;
            for (auto const& aut : model.getAutomata()) {
                if (aut.hasVariable(variable.getName())) {
                    if (foundVar) {
                        return false;
                    }
                    foundVar = true;
                }
            }
            return foundVar;
        }
        
        std::pair<bool, uint64_t> JaniScopeChanger::canMakeVariableLocal(storm::expressions::Variable const& variable, Model const& model, std::vector<Property> const& properties, boost::optional<uint64_t> automatonIndex) const {
            uint64_t index = model.getNumberOfAutomata();
            
            if (!model.getGlobalVariables().hasVariable(variable)) {
                return {false, index};
            }
            
            auto accessingAutomata = detail::getAutomataAccessingVariable(variable, model);
            if (accessingAutomata.size() > 1 || (automatonIndex.is_initialized() && accessingAutomata.count(automatonIndex.get()) == 0)) {
                return {false, index};
            }
            if (model.getInitialStatesRestriction().containsVariable({variable})) {
                return {false, index};
            }
            for (auto const& rewExp : model.getNonTrivialRewardExpressions()) {
                if (rewExp.second.containsVariable({variable})) {
                    return {false, index};
                }
            }
            for (auto const& funDef : model.getGlobalFunctionDefinitions()) {
                if (funDef.second.getFunctionBody().containsVariable({variable})) {
                    return {false, index};
                }
            }
            for (auto const& p : properties) {
                if (p.getUsedVariablesAndConstants().count(variable) > 0) {
                    return {false, index};
                }
                if (p.getUsedLabels().count(variable.getName()) > 0) {
                    return {false, index};
                }
            }
            
            if (accessingAutomata.empty()) {
                index = automatonIndex.is_initialized() ? automatonIndex.get() : 0;
            } else {
                index = *accessingAutomata.begin();
                assert(!automatonIndex.is_initialized() || index == automatonIndex.get());
            }
            return {true, index};
        }
        
        void JaniScopeChanger::makeVariablesGlobal(Model& model) const {
            for (uint64_t i = 0; i < model.getNumberOfAutomata(); ++i) {
                // Make sure to not erase from a set while iterating over it...
                std::set<storm::expressions::Variable> varsToMakeGlobal;
                for (auto const& v : model.getAutomaton(i).getVariables()) {
                    if (canMakeVariableGlobal(v.getExpressionVariable(), model)) {
                        varsToMakeGlobal.insert(v.getExpressionVariable());
                    }
                }
                for (auto const& v : varsToMakeGlobal) {
                    makeVariableGlobal(v, model);
                }
            }
        }
        
        void JaniScopeChanger::makeVariablesLocal(Model& model, std::vector<Property> const& properties) const {
            // Make sure to not erase from a set while iterating over it...
            std::map<storm::expressions::Variable, uint64_t> varsToMakeLocal;
            for (auto const& v : model.getGlobalVariables()) {
                auto makeLocal = canMakeVariableLocal(v.getExpressionVariable(), model, properties);
                if (makeLocal.first) {
                    varsToMakeLocal[v.getExpressionVariable()] = makeLocal.second;
                }
            }
            for (auto const& v : varsToMakeLocal) {
                makeVariableLocal(v.first, model, v.second);
            }
        }
        
    }

}