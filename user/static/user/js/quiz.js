document.addEventListener('DOMContentLoaded', function() {
    // Modal elements
    const modal = document.getElementById('quizModal');
    const startQuizBtn = document.getElementById('startQuiz');
    const browseSuitcasesBtn = document.getElementById('browseSuitcases');
    const closeBtn = document.querySelector('.close');
    const suitcasesList = document.getElementById('suitcasesList');
    const welcomeSection = document.getElementById('welcomeSection');
    const recommendationsSection = document.getElementById('recommendationsSection');
    const notuser = document.querySelector('.notuser');
    const retakeQuizBtn = document.getElementById('retakeQuiz');
    const showAllSuitcasesBtn = document.getElementById('showAllSuitcases');

    // Quiz elements
    const quizSteps = document.querySelectorAll('.quiz-step');
    const prevBtn = document.getElementById('prevBtn');
    const nextBtn = document.getElementById('nextBtn');
    const submitBtn = document.getElementById('submitQuiz');
    const stepIndicator = document.getElementById('stepIndicator');

    let currentStep = 0;
    const userAnswers = {};

    // Modal handlers
    startQuizBtn.addEventListener('click', () => {
        modal.style.display = 'block';
        notuser.style.display = 'none';
        resetQuiz();
    });

    browseSuitcasesBtn.addEventListener('click', () => {
        welcomeSection.style.display = 'none';
        recommendationsSection.style.display = 'none';
        suitcasesList.style.display = 'block';
    });

    closeBtn.addEventListener('click', () => {
        modal.style.display = 'none';
        notuser.style.display = 'flex';
    });

    window.addEventListener('click', (event) => {
        if (event.target === modal) {
            modal.style.display = 'none';
            notuser.style.display = 'flex';
        }
    });

    // New button handlers
    retakeQuizBtn?.addEventListener('click', () => {
        welcomeSection.style.display = 'block';
        recommendationsSection.style.display = 'none';
        suitcasesList.style.display = 'none';
        
        // Clear any stored recommendations
        fetch('/users/clear-recommendations/', {
            method: 'POST',
            headers: {
                'X-CSRFToken': getCSRFToken(),
                'Content-Type': 'application/json',
            }
        });
    });

    showAllSuitcasesBtn?.addEventListener('click', () => {
        recommendationsSection.style.display = 'none';
        suitcasesList.style.display = 'block';
    });

    // Quiz navigation (keep existing code)
    prevBtn.addEventListener('click', () => {
        if (currentStep > 0) {
            currentStep--;
            updateStep();
        }
    });

    nextBtn.addEventListener('click', () => {
        const currentStepId = quizSteps[currentStep].id;
        if (!userAnswers[currentStepId]) {
            alert('Please select an answer before continuing.');
            return;
        }
        
        if (currentStep < quizSteps.length - 1) {
            currentStep++;
            updateStep();
        }
    });

    submitBtn.addEventListener('click', () => {
        const currentStepId = quizSteps[currentStep].id;
        if (!userAnswers[currentStepId]) {
            alert('Please select an answer before submitting.');
            return;
        }
        submitQuiz();
    });

    // Option selection (keep existing code)
    document.addEventListener('click', function(e) {
        if (e.target.classList.contains('option-btn')) {
            const btn = e.target;
            const questionElement = btn.closest('.quiz-step');
            const siblings = questionElement.querySelectorAll('.option-btn');
            siblings.forEach(sibling => sibling.classList.remove('selected'));
            
            btn.classList.add('selected');
            
            const questionId = questionElement.id;
            const categoriesString = btn.dataset.categories;
            userAnswers[questionId] = categoriesString ? categoriesString.split(',') : [];
            
            updateButtonStates();
        }
    });

    function updateStep() {
        quizSteps.forEach(step => step.classList.remove('active'));
        
        if (quizSteps[currentStep]) {
            quizSteps[currentStep].classList.add('active');
        }
        
        stepIndicator.textContent = `Question ${currentStep + 1} of ${quizSteps.length}`;
        updateButtonStates();
    }

    function updateButtonStates() {
        const currentStepId = quizSteps[currentStep] ? quizSteps[currentStep].id : null;
        const isAnswered = currentStepId ? !!userAnswers[currentStepId] : false;
        
        prevBtn.disabled = currentStep === 0;
        
        if (currentStep === quizSteps.length - 1) {
            nextBtn.style.display = 'none';
            submitBtn.style.display = 'inline-block';
            submitBtn.disabled = !isAnswered;
        } else {
            nextBtn.style.display = 'inline-block';
            submitBtn.style.display = 'none';
            nextBtn.disabled = !isAnswered;
        }
    }

    function resetQuiz() {
        currentStep = 0;
        Object.keys(userAnswers).forEach(key => delete userAnswers[key]);
        
        const optionBtns = document.querySelectorAll('.option-btn');
        optionBtns.forEach(btn => btn.classList.remove('selected'));
        
        updateStep();
    }

    function submitQuiz() {
        const allSelectedCategories = [];

        Object.values(userAnswers).forEach(categories => {
            if (Array.isArray(categories)) {
                allSelectedCategories.push(...categories);
            }
        });
        
        console.log('Selected Categories:', allSelectedCategories);
        
        // Show loading state
        submitBtn.disabled = true;
        submitBtn.textContent = 'Finding your perfect match...';
        
        // Send to server for Jaccard calculation
        fetch(window.location.href, {
            method: 'POST',
            headers: {
                'X-Requested-With': 'XMLHttpRequest',
                'X-CSRFToken': getCSRFToken(),
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                categories: allSelectedCategories
            })
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                // Show recommendations
                modal.style.display = 'none';
                welcomeSection.style.display = 'none';
                recommendationsSection.style.display = 'block';
                suitcasesList.style.display = 'none';
                notuser.style.display = 'flex';
                
                // Update recommendations container with new data
                updateRecommendationsDisplay(data.recommendations);
            } else {
                alert('Error calculating recommendations: ' + data.error);
                resetSubmitButton();
            }
        })
        .catch(error => {
            console.error('Error:', error);
            alert('Error calculating recommendations');
            resetSubmitButton();
        });
    }

    function updateRecommendationsDisplay(recommendations) {
        const container = document.getElementById('recommendationsContainer');
        
        if (recommendations.length === 0) {
            container.innerHTML = `
                <div class="no-recommendations">
                    <h3>No perfect matches found</h3>
                    <p>We couldn't find suitcases that match your criteria. Try browsing all suitcases instead.</p>
                </div>
            `;
            return;
        }
        
        container.innerHTML = recommendations.map(rec => `
            <div class="recommendation-item">
                <div class="recommendation-header">
                    <h3>${rec.name}</h3>
                    <span class="match-score">${rec.score}% Match</span>
                </div>
                <div class="recommendation-categories">
                    <strong>Features:</strong>
                    ${rec.categories.map(cat => `<span class="category-tag">${cat}</span>`).join('')}
                </div>
                <a href="/suitcases/${rec.uuid}" class="recommendation-btn">View Suitcase</a>
            </div>
        `).join('');
    }

    function resetSubmitButton() {
        submitBtn.disabled = false;
        submitBtn.textContent = 'See Recommendations';
    }

    // Helper function to get CSRF token
    function getCSRFToken() {
        const name = 'csrftoken';
        let cookieValue = null;
        if (document.cookie && document.cookie !== '') {
            const cookies = document.cookie.split(';');
            for (let i = 0; i < cookies.length; i++) {
                const cookie = cookies[i].trim();
                if (cookie.substring(0, name.length + 1) === (name + '=')) {
                    cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
                    break;
                }
            }
        }
        return cookieValue;
    }
});