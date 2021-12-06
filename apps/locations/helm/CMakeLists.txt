if(ASSFIRE_HELM_ENABLED)
    include(helm_utils)

    define_helm_target(
            TARGET_NAME assfire-locations-helm
            CHART_FILES
            assfire-locations/Chart.yaml
            assfire-locations/values.yaml
            assfire-locations/templates/deployment.yaml
            assfire-locations/templates/ingress.yaml
            assfire-locations/templates/service.yaml
            CHART_ROOT assfire-locations
            INSTALL_RELEASE_NAME assfire-locations
    )
endif()