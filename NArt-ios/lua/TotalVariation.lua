local TVLoss, parent = torch.class('nn.TotalVariation', 'nn.Module')


function TVLoss:__init(strength)
  parent.__init(self)
  self.strength = strength
  self.x_diff = torch.Tensor()
  self.y_diff = torch.Tensor()
end


function TVLoss:updateOutput(input)
  self.output = input
  return self.output
end


