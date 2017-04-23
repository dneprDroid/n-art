require 'torch'
require 'nn'
require 'image'

--require 'nn.SpatialReflectionPadding'

require 'ShaveImage'
require 'TotalVariation'
require 'InstanceNormalization'

--require 'nn.SpatialBatchNormalization'
--require 'nn.MulConstant'


-- local utils = require 'fast_neural_style.utils'
-- local preprocess = require 'fast_neural_style.preprocess'

--[[
Use a trained feedforward model to stylize either a single image or an entire
directory of images.
--]]



local vgg_mean = { 103.939, 116.779, 123.68 }

--local image_size = 768
--local m_filter = 3

--- **** Torch Predictor **** ----
        local model -- torch pre-trained model

        function initPredictor(path)
            print('Lua: started initing predictor')

            model = torch.load(path).model
            print(model)
            -- model:evaluate()
            model:type('torch.FloatTensor')
            print('Lua: ended initing predictor')

            -- print('Lua: img start forward....')
            -- local inputImage = getTensorImage()
            -- print('Lua: img forward....')

            -- local img_out = model:forward(img_pre)
            -- print('Lua: img forward ended....')
            -- saveImageTensor(img_out)

        end

        function styleImage(img_pre)
            print('Lua: started image forwarding.......')
            print(  type(img_pre)  )
            local img_out = model:forward(img_pre)
            print('Lua: ended image forwarding')
            return img_out
        end

--- **** Torch Predictor **** ----
