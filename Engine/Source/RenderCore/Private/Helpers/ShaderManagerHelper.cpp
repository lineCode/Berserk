//
// Created by Egor Orachyov on 22.03.2019.
//

#include "Helpers/ProfileHelpers.h"
#include "Helpers/ShaderManagerHelper.h"
#include "Misc/FileUtility.h"

namespace Berserk::Resources
{

    bool ShaderManagerHelper::import(IShader *shader, XMLNode &node, const CString &path)
    {
        bool compiled = false;
        uint32 attachments = 0;

        for (auto block = node.getChild(); !block.isEmpty(); block = block.getNext())
        {
            if (CName("shaders") == block.getName())
            {
                if (compiled)
                {
                    return false;
                }

                for (auto current = block.getChild(); !current.isEmpty(); current = current.getNext())
                {
                    if (CName("shader") == current.getName())
                    {
                        CPath filename(current.getAttribute("path").getValue());
                        filename = filename.replace(CPath("{SHADERS}"), CPath(path.get()));

                        const uint32 size = Buffers::KiB * 20;
                        char buffer[size];
                        FileUtility::read(filename.get(), buffer, size);

                        shader->attachShader(getShaderType(current.getAttribute("type").getValue()),
                                             buffer,
                                             filename.get());

#if PROFILE_SHADER_MANAGER_HELPER
                        PUSH("ShaderManagerHelper: Compile from path [path: '%s'] \n", filename.get());
#endif

                        attachments += 1;
                    }
                    else
                    {
                        WARNING("Unknown node in XML node parsing for program [name: '%s']", current.getName());
                    }
                }

                if (attachments == 0)
                {
                    return false;
                }

                shader->link();
                compiled = true;
                continue;
            }

            if (compiled)
            {
                if (CName("uniforms") == block.getName())
                {
                    for (auto current = block.getChild(); !current.isEmpty(); current = current.getNext())
                    {
                        if (CName("uniform") == current.getName())
                        {
                            shader->addUniformVariable(current.getAttribute("name").getValue());
#if PROFILE_SHADER_MANAGER_HELPER
                            PUSH("ShaderManagerHelper: Uniform variable [name: '%s'] \n", current.getAttribute("name").getValue());
#endif
                        }
                        else
                        {
                            WARNING("Unknown node in XML node parsing for program [name: '%s']", current.getName());
                        }
                    }
                }
                else if (CName("uniformblocks") == block.getName())
                {
                    for (auto current = block.getChild(); !current.isEmpty(); current = current.getNext())
                    {
                        if (CName("uniformblock") == current.getName())
                        {
                            const char* name = current.getAttribute("name").getValue();
                            const char* binding = current.getAttribute("binding").getValue();
                            const auto  binding_point = (uint32) atoi(binding);

                            shader->setUniformBlockBinding(name, binding_point);
#if PROFILE_SHADER_MANAGER_HELPER
                            PUSH("ShaderManagerHelper: Uniform block [name: '%s'][binding: %u] \n", current.getAttribute("name").getValue(), binding_point);
#endif
                        }
                        else
                        {
                            WARNING("Unknown node in XML node parsing for program [name: '%s']", current.getName());
                        }
                    }
                }
                else if (CName("subroutine") == block.getName())
                {

                }
                else
                {
                    WARNING("Unknown node in XML node parsing for program [name: '%s']", block.getName());
                }
            }
        }

        return true;
    }

    IRenderDriver::ShaderType ShaderManagerHelper::getShaderType(const char *string)
    {
        if (CName("VERTEX") == string)
        {
            return IRenderDriver::ShaderType::VERTEX;
        }
        else if (CName("TESSELLATION_CONTROL") == string)
        {
            return IRenderDriver::ShaderType::TESSELLATION_CONTROL;
        }
        else if (CName("TESSELLATION_EVALUATION") == string)
        {
            return IRenderDriver::ShaderType::TESSELLATION_EVALUATION;
        }
        else if (CName("GEOMETRY") == string)
        {
            return IRenderDriver::ShaderType::GEOMETRY;
        }
        else if (CName("FRAGMENT") == string)
        {
            return IRenderDriver::ShaderType::FRAGMENT;
        }
        else if (CName("COMPUTE") == string)
        {
            return IRenderDriver::ShaderType::COMPUTE;
        }

        WARNING("Cannot find shader type from string [string: '%s']", string);
        return IRenderDriver::NOT_SHADER_TYPE;
    }

} // namespace Berserk::Resources