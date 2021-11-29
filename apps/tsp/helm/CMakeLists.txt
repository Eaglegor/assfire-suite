if(ASSFIRE_HELM_ENABLED)
    include(helm_utils)

    define_helm_target(
            TARGET_NAME assfire-tsp-helm
            CHART_FILES
            assfire-tsp/Chart.yaml
            assfire-tsp/values.yaml
            assfire-tsp/templates/deployment-server.yaml
            assfire-tsp/templates/deployment-worker.yaml
            assfire-tsp/templates/ingress.yaml
            assfire-tsp/templates/service.yaml
            CHART_ROOT assfire-tsp
            INSTALL_RELEASE_NAME assfire-tsp
    )
endif()