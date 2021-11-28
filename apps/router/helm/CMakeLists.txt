if(ASSFIRE_HELM_ENABLED)
    include(helm_utils)

    define_helm_target(
            TARGET_NAME assfire-router-helm
            CHART_FILES
            assfire-router/Chart.yaml
            assfire-router/values.yaml
            assfire-router/templates/deployment-server.yaml
            assfire-router/templates/ingress-server.yaml
            assfire-router/templates/service-server.yaml
            assfire-router/templates/deployment-maptest.yaml
            assfire-router/templates/ingress-maptest.yaml
            assfire-router/templates/service-maptest.yaml
            CHART_ROOT assfire-router
            INSTALL_RELEASE_NAME assfire-router
    )
endif()