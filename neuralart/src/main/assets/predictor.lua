-- require 'torch'
-- require 'nn'
-- require 'image'

-- require 'fast_neural_style.ShaveImage'
-- require 'fast_neural_style.TotalVariation'
-- require 'fast_neural_style.InstanceNormalization'

-- local utils = require 'fast_neural_style.utils'
-- local preprocess = require 'fast_neural_style.preprocess'

--[[
Use a trained feedforward model to stylize either a single image or an entire
directory of images.
--]]



local vgg_mean = { 103.939, 116.779, 123.68 }

local image_size = 768
local m_filter = 3

--- **** Torch Predictor **** ----
        local model -- torch pre-trained model

        local function initPredictor()
            print('started initing predictor')
            -- model = torch.load('candy.t7').model
            -- print(model)
            -- model:evaluate()
            -- model:type('torch.FloatTensor')
            print('ended initing predictor')
        end

        local function styleImage(img_pre)
            print('started image forwarding.......')
            -- local img_out = model:forward(img_pre)
            -- print('ended image forwarding')
            -- return img_out
            return img_pre
        end

--- **** Torch Predictor **** ----
