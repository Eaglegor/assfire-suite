set (CHART_FILES
        assfire-router/Chart.yaml
        assfire-router/values.yaml
        assfire-router/templates/deployment-server.yaml
        assfire-router/templates/ingress-server.yaml
        assfire-router/templates/service-server.yaml
        assfire-router/templates/deployment-maptest.yaml
        assfire-router/templates/ingress-maptest.yaml
        assfire-router/templates/service-maptest.yaml
        )

foreach(file ${CHART_FILES})
    configure_file(${file} ${file} @ONLY)
endforeach()