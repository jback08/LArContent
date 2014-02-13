/**
 *  @file   LArContent/src/LArUtility/ListMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the list merging algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LArUtility/ListMergingAlgorithm.h"

using namespace pandora;

namespace lar
{

StatusCode ListMergingAlgorithm::Run()
{
    // Cluster list merging
    if (m_sourceClusterListNames.size() != m_targetClusterListNames.size())
        return STATUS_CODE_FAILURE;

    for (unsigned int iIndex = 0, iIndexEnd = m_sourceClusterListNames.size(); iIndex < iIndexEnd; ++iIndex)
    {
        const std::string &sourceListName(m_sourceClusterListNames.at(iIndex));
        const std::string &targetListName(m_targetClusterListNames.at(iIndex));

        const StatusCode statusCode(PandoraContentApi::SaveClusterList(*this, sourceListName, targetListName));

        if (STATUS_CODE_SUCCESS != statusCode)
        {
            if (STATUS_CODE_NOT_FOUND == statusCode)
            {
                std::cout << "ListMergingAlgorithm: Cluster list not found, source: " << sourceListName << ", target: " << targetListName << std::endl;
            }
            else if (STATUS_CODE_NOT_INITIALIZED == statusCode)
            {
                std::cout << "ListMergingAlgorithm: No clusters to move, source: " << sourceListName << ", target: " << targetListName << std::endl;
            }
            else
            {
                std::cout << "ListMergingAlgorithm: Error in cluster merging, source: " << sourceListName << ", target: " << targetListName << std::endl;
                return statusCode;
            }
        }
    }

    // Pfo list merging
    if (m_sourcePfoListNames.size() != m_targetPfoListNames.size())
        return STATUS_CODE_FAILURE;

    for (unsigned int iIndex = 0, iIndexEnd = m_sourcePfoListNames.size(); iIndex < iIndexEnd; ++iIndex)
    {
        const std::string &sourceListName(m_sourcePfoListNames.at(iIndex));
        const std::string &targetListName(m_targetPfoListNames.at(iIndex));

        const StatusCode statusCode(PandoraContentApi::SavePfoList(*this, sourceListName, targetListName));

        if (STATUS_CODE_SUCCESS != statusCode)
        {
            if (STATUS_CODE_NOT_FOUND == statusCode)
            {
                std::cout << "ListMergingAlgorithm: Pfo list not found, source: " << sourceListName << ", target: " << targetListName << std::endl;
            }
            else if (STATUS_CODE_NOT_INITIALIZED == statusCode)
            {
                std::cout << "ListMergingAlgorithm: No pfos to move, source: " << sourceListName << ", target: " << targetListName << std::endl;
            }
            else
            {
                std::cout << "ListMergingAlgorithm: Error in pfo merging, source: " << sourceListName << ", target: " << targetListName << std::endl;
                return statusCode;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ListMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_sourceClusterListNames.clear();
    m_targetClusterListNames.clear();
    m_sourcePfoListNames.clear();
    m_targetPfoListNames.clear();

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "SourceClusterListNames", m_sourceClusterListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "TargetClusterListNames", m_targetClusterListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "SourcePfoListNames", m_sourcePfoListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "TargetPfoListNames", m_targetPfoListNames));

    if ((m_sourceClusterListNames.size() != m_targetClusterListNames.size()) || (m_sourcePfoListNames.size() != m_targetPfoListNames.size()))
    {
        std::cout << "ListMergingAlgorithm::ReadSettings - Invalid list configuration " << std::endl;
        return STATUS_CODE_INVALID_PARAMETER;
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace lar
