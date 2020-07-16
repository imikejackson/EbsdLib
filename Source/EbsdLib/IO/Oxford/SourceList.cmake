set(OXFORD_SRCS
  )

set(OXFORD_HDRS
  )


if(EbsdLib_ENABLE_HDF5)
    set(OXFORD_SRCS ${OXFORD_SRCS}
      ${EbsdLibProj_SOURCE_DIR}/Source/EbsdLib/IO/Oxford/OinaPhase.cpp
      ${EbsdLibProj_SOURCE_DIR}/Source/EbsdLib/IO/Oxford/H5OinaReader.cpp
      ${EbsdLibProj_SOURCE_DIR}/Source/EbsdLib/IO/Oxford/H5OinaFields.cpp
    )
    set(OXFORD_HDRS ${OXFORD_HDRS}
      ${EbsdLibProj_SOURCE_DIR}/Source/EbsdLib/IO/Oxford/OinaPhase.h
      ${EbsdLibProj_SOURCE_DIR}/Source/EbsdLib/IO/Oxford/H5OinaReader.h
      ${EbsdLibProj_SOURCE_DIR}/Source/EbsdLib/IO/Oxford/H5OinaFields.h
    )
endif()


# cmp_IDE_SOURCE_PROPERTIES( "Oxford" "${OXFORD_HDRS}" "${OXFORD_SRCS}" ${EbsdLib_INSTALL_FILES})

if( ${EbsdLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${OXFORD_HDRS}
            DESTINATION include/EbsdLib/IO/Oxford
            COMPONENT Headers   )
endif()


