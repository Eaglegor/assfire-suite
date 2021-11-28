if(ASSFIRE_HELM_ENABLED)
    include(helm_utils)

    define_helm_target(
            TARGET_NAME assfire-scheduler-helm
            CHART_FILES
            assfire-scheduler/Chart.yaml
            assfire-scheduler/values.yaml
            assfire-scheduler/templates/deployment.yaml
            assfire-scheduler/templates/ingress.yaml
            assfire-scheduler/templates/service.yaml
            CHART_ROOT assfire-scheduler
            INSTALL_RELEASE_NAME assfire-scheduler
    )
endif()