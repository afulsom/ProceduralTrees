#include "Engine/Core/LSystem.hpp"
#include "WeightedList.hpp"


LSystem::LSystem()
	: m_axiom()
	, m_rules()
{

}

LSystem::~LSystem()
{

}

std::string LSystem::Run(unsigned int numIterations)
{
	std::string system = m_axiom;
	for (unsigned int iteration = 0; iteration < numIterations; ++iteration)
	{
		size_t systemSize = system.size();
		for (size_t characterIndex = 0; characterIndex < systemSize; characterIndex++)
		{
			std::pair<std::map<char, Rule>::iterator, std::map<char, Rule>::iterator> foundRule = m_rules.equal_range(system.at(characterIndex));

			WeightedList<std::string> potentialReplacementStrings;
			for (std::map<char, Rule>::iterator iter = foundRule.first; iter != foundRule.second; iter++)
			{
				potentialReplacementStrings.AddItem(iter->second.m_stringToReplaceWith, iter->second.m_weight);
			}

			if (!potentialReplacementStrings.IsEmpty())
			{
				std::string replacementString = potentialReplacementStrings.GetWeightedRandomItem();

				system.replace(characterIndex, 1, replacementString);
				characterIndex += (replacementString.size() - 1);
				systemSize += (replacementString.size() - 1);
			}
		}
	}

	return system;
}

std::string LSystem::Run()
{
	std::string system = m_axiom;
	for (unsigned int iteration = 0; iteration < m_numIterations; ++iteration)
	{
		size_t systemSize = system.size();
		for (size_t characterIndex = 0; characterIndex < systemSize; characterIndex++)
		{
			std::pair<std::map<char, Rule>::iterator, std::map<char, Rule>::iterator> foundRule = m_rules.equal_range(system.at(characterIndex));

			WeightedList<std::string> potentialReplacementStrings;
			for (std::map<char, Rule>::iterator iter = foundRule.first; iter != foundRule.second; iter++)
			{
				potentialReplacementStrings.AddItem(iter->second.m_stringToReplaceWith, iter->second.m_weight);
			}

			if (!potentialReplacementStrings.IsEmpty())
			{
				std::string replacementString = potentialReplacementStrings.GetWeightedRandomItem();

				system.replace(characterIndex, 1, replacementString);
				characterIndex += (replacementString.size() - 1);
				systemSize += (replacementString.size() - 1);
			}
		}
	}

	return system;
}
