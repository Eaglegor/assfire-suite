include(helm_utils)
prepare_helm_chart(
        CHART_ROOT assfire-router
        VERSION ${ASSFIRE_APPLICATION_RELEASE_VERSION}
)
