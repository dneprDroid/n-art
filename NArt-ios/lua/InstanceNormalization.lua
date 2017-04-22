require 'nn'

--[[
  Implements instance normalization as described in the paper
  
  Instance Normalization: The Missing Ingredient for Fast Stylization
  Dmitry Ulyanov, Andrea Vedaldi, Victor Lempitsky
  https://arxiv.org/abs/1607.08022

  This implementation is based on
  https://github.com/DmitryUlyanov/texture_nets
]]

local InstanceNormalization, parent = torch.class('nn.InstanceNormalization',
                                                  'nn.Module')


function InstanceNormalization:__init(nOutput, eps)
  parent.__init(self)

  self.eps = eps or 1e-5
  
  self.nOutput = nOutput
  self.prev_N = -1

  self.weight = torch.Tensor(nOutput):uniform()
  self.bias = torch.Tensor(nOutput):zero()
  self.gradWeight = torch.Tensor(nOutput)
  self.gradBias = torch.Tensor(nOutput)
end


function InstanceNormalization:updateOutput(input)
  local N, C = input:size(1), input:size(2)
  local H, W = input:size(3), input:size(4)
  assert(C == self.nOutput)

  if N ~= self.prev_N or (self.bn and self:type() ~= self.bn:type()) then
    self.bn = nn.SpatialBatchNormalization(N * C, self.eps)
    self.bn:type(self:type())
    self.prev_N = N
  end

  -- Set params for BN
  self.bn.weight:repeatTensor(self.weight, N)
  self.bn.bias:repeatTensor(self.bias, N)

  local input_view = input:view(1, N * C, H, W)
  self.bn:training()
  self.output = self.bn:forward(input_view):viewAs(input)

  return self.output
end


