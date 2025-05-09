#include "VariableNotAMooseObjectAction.h"

#include "AddVariableAction.h"
#include "FEProblemBase.h"

#include "libmesh/string_to_enum.h"
#include "libmesh/fe_type.h"

InputParameters
VariableNotAMooseObjectAction::validParams()
{
  InputParameters params = Action::validParams();

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
  if (isParamValid("block"))
    for (const auto & subdomain_name : getParam<std::vector<SubdomainName>>("block"))
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
  auto fe_type = AddVariableAction::feType(_pars);
  auto type = AddVariableAction::variableType(fe_type);
  auto var_params = _factory.getValidParams(type);
  var_params.applySpecificParameters(_pars, {"family", "order"});
  var_params.set<std::vector<Real>>("scaling") = {getParam<Real>("scaling")};

  if (blocks.empty())
    _problem->addVariable(type, var_name, var_params);

  else
  {
    for (const SubdomainID & id : blocks)
      var_params.set<std::vector<SubdomainName>>("block").push_back(Moose::stringify(id));

    _problem->addVariable(type, var_name, var_params);
  }
}
