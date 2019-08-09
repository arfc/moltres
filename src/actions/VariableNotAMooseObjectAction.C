#include "VariableNotAMooseObjectAction.h"

#include "AddVariableAction.h"
#include "FEProblemBase.h"

#include "libmesh/string_to_enum.h"
#include "libmesh/fe_type.h"

template <>
InputParameters
validParams<VariableNotAMooseObjectAction>()
{
  InputParameters params = validParams<Action>();

  // Get MooseEnums for the possible order/family options for this variable
  MooseEnum families(AddVariableAction::getNonlinearVariableFamilies());
  MooseEnum orders(AddVariableAction::getNonlinearVariableOrders());

  // add_variable parameters
  params.addParam<MooseEnum>(
      "family", families, "Specifies the family of FE shape functions to use for this variable");
  params.addParam<MooseEnum>("order",
                             orders,
                             "Specifies the order of the FE shape function to use "
                             "for this variable (additional orders not listed are "
                             "allowed)");
  params.addParam<Real>("scaling", 1.0, "Specifies a scaling factor to apply to this variable");
  params.addParam<std::vector<SubdomainName>>("block", "The block id where this variable lives");
  return params;
}

VariableNotAMooseObjectAction::VariableNotAMooseObjectAction(const InputParameters & params)
  : Action(params)
{
}

std::set<SubdomainID>
VariableNotAMooseObjectAction::getSubdomainIDs()
{
  // Extract and return the block ids supplied in the input
  std::set<SubdomainID> blocks;
  std::vector<SubdomainName> block_param = getParam<std::vector<SubdomainName>>("block");
  for (const auto & subdomain_name : block_param)
  {
    SubdomainID blk_id = _problem->mesh().getSubdomainID(subdomain_name);
    blocks.insert(blk_id);
  }
  return blocks;
}

void
VariableNotAMooseObjectAction::addVariable(const std::string & var_name)
{
  std::set<SubdomainID> blocks = getSubdomainIDs();
  if (blocks.empty())
    _problem->addVariable(var_name,
                          FEType(Utility::string_to_enum<Order>(getParam<MooseEnum>("order")),
                                 Utility::string_to_enum<FEFamily>(getParam<MooseEnum>("family"))),
                          getParam<Real>("scaling"));
  else
    _problem->addVariable(var_name,
                          FEType(Utility::string_to_enum<Order>(getParam<MooseEnum>("order")),
                                 Utility::string_to_enum<FEFamily>(getParam<MooseEnum>("family"))),
                          getParam<Real>("scaling"),
                          &blocks);
}
