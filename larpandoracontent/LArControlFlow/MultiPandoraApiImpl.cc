/**
 *  @file   larpandoracontent/LArControlFlow/MultiPandoraApiImpl.cc
 *
 *  @brief  Implementation of the MultiPandoraApiImpl class.
 *
 *  $Log: $
 */

#include "Pandora/Pandora.h"
#include "Pandora/StatusCodes.h"

#include "larpandoracontent/LArControlFlow/MultiPandoraApiImpl.h"

const PandoraInstanceMap &MultiPandoraApiImpl::GetPandoraInstanceMap() const
{
    return m_primaryToDaughtersMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const pandora::Pandora *MultiPandoraApiImpl::GetPandoraInstance(const pandora::Pandora *const pPrimaryPandora, const unsigned int volumeId) const
{
    PandoraInstanceList instanceList(this->GetDaughterPandoraInstanceList(pPrimaryPandora));
    instanceList.push_back(pPrimaryPandora);

    for (const pandora::Pandora *const pPandora : instanceList)
    {
        try
        {
            if (volumeId == this->GetVolumeId(pPandora))
                return pPandora;
        }
        catch (const pandora::StatusCodeException &)
        {
        }
    }

    throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_FOUND);
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraInstanceList &MultiPandoraApiImpl::GetDaughterPandoraInstanceList(const pandora::Pandora *const pPrimaryPandora) const
{
    PandoraInstanceMap::const_iterator iter = m_primaryToDaughtersMap.find(pPrimaryPandora);

    if (m_primaryToDaughtersMap.end() == iter)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_FOUND);

    return iter->second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const pandora::Pandora *MultiPandoraApiImpl::GetPrimaryPandoraInstance(const pandora::Pandora *const pDaughterPandora) const
{
    PandoraRelationMap::const_iterator iter = m_daughterToPrimaryMap.find(pDaughterPandora);

    if (m_daughterToPrimaryMap.end() == iter)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_FOUND);

    return iter->second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int MultiPandoraApiImpl::GetVolumeId(const pandora::Pandora *const pPandora) const
{
    PandoraToVolumeIdMap::const_iterator iter = m_pandoraToVolumeIdMap.find(pPandora);

    if (m_pandoraToVolumeIdMap.end() == iter)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_FOUND);

    return iter->second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MultiPandoraApiImpl::SetVolumeId(const pandora::Pandora *const pPandora, const unsigned int volumeId)
{
    if (m_pandoraToVolumeIdMap.count(pPandora))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_ALREADY_PRESENT);

    if (!m_pandoraToVolumeIdMap.insert(PandoraToVolumeIdMap::value_type(pPandora, volumeId)).second)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

MultiPandoraApiImpl::MultiPandoraApiImpl()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

MultiPandoraApiImpl::~MultiPandoraApiImpl()
{
    // ATTN This is a copy of the input map, which will be modified by calls to delete pandora instances
    PandoraInstanceMap pandoraInstanceMap(m_primaryToDaughtersMap);

    for (const auto &mapElement : pandoraInstanceMap)
        this->DeletePandoraInstances(mapElement.first);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MultiPandoraApiImpl::AddPrimaryPandoraInstance(const pandora::Pandora *const pPrimaryPandora)
{
    if (!m_primaryToDaughtersMap.insert(PandoraInstanceMap::value_type(pPrimaryPandora, PandoraInstanceList())).second)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_ALREADY_PRESENT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MultiPandoraApiImpl::AddDaughterPandoraInstance(const pandora::Pandora *const pPrimaryPandora, const pandora::Pandora *const pDaughterPandora)
{
    PandoraInstanceMap::iterator iter = m_primaryToDaughtersMap.find(pPrimaryPandora);

    if (m_primaryToDaughtersMap.end() == iter)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_FOUND);

    iter->second.push_back(pDaughterPandora);

    if (!m_daughterToPrimaryMap.insert(PandoraRelationMap::value_type(pDaughterPandora, pPrimaryPandora)).second)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MultiPandoraApiImpl::DeletePandoraInstances(const pandora::Pandora *const pPrimaryPandora, bool deletePrimary)
{
    PandoraInstanceList pandoraInstanceList = this->GetDaughterPandoraInstanceList(pPrimaryPandora);

    for (const pandora::Pandora *const pPandora : pandoraInstanceList)
    {
        // Does this have any daughters? If so, recursively delete them
        if (m_primaryToDaughtersMap.find(pPandora) != m_primaryToDaughtersMap.end())
        {
            // pPandora becomes a "primary", but we don't want to delete it yet
            this->DeletePandoraInstances(pPandora, false);
        }

        // Now delete pPandora
        m_pandoraToVolumeIdMap.erase(pPandora);
        m_daughterToPrimaryMap.erase(pPandora);
        // Delete its entry in the primary-to-daughters map if requested
        if (deletePrimary)
            m_primaryToDaughtersMap.erase(pPandora);

        delete pPandora;
    }

    // Now delete the primary instance if requested
    if (deletePrimary)
    {
        m_pandoraToVolumeIdMap.erase(pPrimaryPandora);
        m_daughterToPrimaryMap.erase(pPrimaryPandora);
        m_primaryToDaughtersMap.erase(pPrimaryPandora);
        delete pPrimaryPandora;
    }
}
