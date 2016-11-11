#include "src/settings/modules/PGCLSettings.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"

#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            const std::string PGCLSettings::moduleName = "pgcl";
            
            const std::string PGCLSettings::pgclFileOptionName = "pgclfile";
            const std::string PGCLSettings::pgclFileOptionShortName = "pgcl";
            const std::string PGCLSettings::pgclToJaniOptionName = "to-jani";
            const std::string PGCLSettings::pgclToJaniOptionShortName = "tj";
            const std::string PGCLSettings::programGraphToDotOptionName = "draw-program-graph";
            const std::string PGCLSettings::programGraphToDotShortOptionName = "pg";
            const std::string PGCLSettings::programVariableRestrictionsOptionName = "variable-restrictions";
            const std::string PGCLSettings::programVariableRestrictionShortOptionName = "rvar";
            
            
            PGCLSettings::PGCLSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, pgclFileOptionName, false, "Parses the pgcl program.").setShortName(pgclFileOptionShortName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, pgclToJaniOptionName, false, "Transform to JANI.").setShortName(pgclToJaniOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, programGraphToDotOptionName, false, "Destination for the program graph dot output.").setShortName(programGraphToDotShortOptionName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, programVariableRestrictionsOptionName, false, "Restrictions of program variables").setShortName(programVariableRestrictionShortOptionName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("description", "description of the variable restrictions").build()).build());
            }
            
            bool PGCLSettings::isPgclFileSet() const {
                return this->getOption(pgclFileOptionName).getHasOptionBeenSet();
            }
            
            std::string PGCLSettings::getPgclFilename() const {
                return this->getOption(pgclFileOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool PGCLSettings::isToJaniSet() const {
                return this->getOption(pgclToJaniOptionName).getHasOptionBeenSet();
            }
            
            bool PGCLSettings::isProgramGraphToDotSet() const {
                return this->getOption(programGraphToDotOptionName).getHasOptionBeenSet();
            }
            
            std::string PGCLSettings::getProgramGraphDotOutputFilename() const {
                return this->getOption(programGraphToDotOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool PGCLSettings::isProgramVariableRestrictionSet() const {
                return this->getOption(programVariableRestrictionsOptionName).getHasOptionBeenSet();
            }
            
            std::string PGCLSettings::getProgramVariableRestrictions() const {
                return this->getOption(programVariableRestrictionsOptionName).getArgumentByName("description").getValueAsString();
            }
            
            void PGCLSettings::finalize() {
                
            }
            
            bool PGCLSettings::check() const {
                return true;
            }
        }
    }
}