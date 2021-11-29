set(CHART_FILES
        assfire-minikube-cert/Chart.yaml
        assfire-minikube-cert/values.yaml
        assfire-minikube-cert/templates/certificate.yaml
        assfire-minikube-cert/templates/issuer.yaml
        )

foreach (file ${CHART_FILES})
    configure_file(${file} ${file} @ONLY)
endforeach ()