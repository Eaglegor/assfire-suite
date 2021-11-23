set (CHART_FILES
        assfire-router/Chart.yaml
        assfire-router/values.yaml
        assfire-router/templates/deployment.yaml
        assfire-router/templates/ingress.yaml
        assfire-router/templates/service.yaml
        )

foreach(file ${CHART_FILES})
    configure_file(${file} ${file} @ONLY)
endforeach()