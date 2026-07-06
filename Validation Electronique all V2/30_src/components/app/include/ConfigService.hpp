// ============================================================================
// FILE: components/app/include/ConfigService.hpp
// LAYER: APP (service métier)
// PURPOSE: Gère les seuils modifiables sans reprogrammation (EXF-23)
// STATUT: SQUELETTE — s'appuiera sur low::FlashStorage
// ============================================================================

#ifndef CONFIG_SERVICE_HPP
#define CONFIG_SERVICE_HPP

#include <cstdint>
#include "common_types.hpp"
#include "flash_storage.hpp"

namespace app {

class ConfigService {
public:
    explicit ConfigService(low::FlashStorage& flash);

    common::Status init();

    // Seuils modifiables (chargés depuis la flash au démarrage)
    int32_t getCurrentThresholdMa() const;
    void    setCurrentThresholdMa(int32_t value);

private:
    low::FlashStorage& m_flash;
    int32_t m_currentThresholdMa;
};

} // namespace app

#endif // CONFIG_SERVICE_HPP
